#include "client/systems/NetworkSystem.hpp"
#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/net/NetClientManager.hpp"
#include "client/predict_client.hpp"
#include "client/voxel/ClientChunkManager.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;


template<>
struct fmt::formatter<entt::entity> : formatter<size_t>{
	auto format(entt::entity c, format_context& ctx){
		return formatter<size_t>::format((size_t)c, ctx);
	}
};

NetworkSystem::NetworkSystem() {
	
}

void NetworkSystem::init(Engine& engine) {

}

void NetworkSystem::gameTick(Engine& engine) {
	using namespace Net;

	Net::NetClientManager& netManager = engine.getNetManager();

	auto& registry = engine.getRegistry();
	entt::entity player = engine.getPlayer();

	const auto& transform = registry.get<Transformation>(player);
	const auto& state = registry.get<MovementState>(player);
	const auto& input = registry.get<Input>(player);
	Net::SB_PlayerInput packet;
	packet.forward = (i8)state.forward;
	packet.strafe = (i8)state.strafe;
	packet.fly = (i8)state.fly;
	packet.client_time = engine.getClientSimulationTime();
	packet.rot = transform.rot;
	
	netManager.appendRequest(state, transform, engine.getClientSimulationTime());
	if (state.fly != 0 || state.forward != 0 || state.strafe != 0 || input.current_cursor_pos != input.last_cursor_pos) {
		netManager.sendMessage(packet, false);
	}

	netManager.poll([&](std::string_view sv) {
		this->onMessage(engine, sv);
	});


	auto network_view = registry.view<Transformation, Network>();
	for (entt::entity clientID : network_view) {
		simulation_duration client_time = engine.getClientSimulationTime();
		std::optional<PositionNetValues> values = netManager.getInterpolatedEntityValues(clientID, client_time);

		if (values) {
			auto& transform = network_view.get<Transformation>(clientID);
			transform.pos = values->pos;
			transform.rot = values->rot;
		}
	}
}

void NetworkSystem::onMessage(Engine& engine, std::string_view data) {
	using namespace Net;

	if (data.size() == 0) {
		spdlog::warn("Packet with zero size recieved");
		return;
	}

	PacketID packetID = static_cast<PacketID>(data[0]);
	data.remove_prefix(1);

	switch (packetID) {
	case PacketID::CB_AssignNetID:
		onAssignNetID(engine, data);
		break;
	case PacketID::CB_SyncSimulationClock:
		onSyncSimulationClock(engine, data);
		break;
	case PacketID::CB_EntityPositionVel:
		onEntityPositionVel(engine, data);
		break;
	case PacketID::CB_EntitySnapshotDelta:
		onEntitySnapshotDelta(engine, data);
		break;
	case PacketID::CB_PlayerPositionAck:
		onPlayerPositionAck(engine, data);
		break;
	case PacketID::CB_PlayerJoin:
		onPlayerJoin(engine, data);
		break;
	case PacketID::CB_SpawnPosition:
		onSpawnPosition(engine, data);
		break;
	case PacketID::CB_ChunkData:
		onChunkData(engine, data);
		break;
	default:
		break;
	}
}

void NetworkSystem::onAssignNetID(Engine& engine, std::string_view data) {
	Net::CB_AssignNetIDPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		spdlog::info("Assigned Net ID: [{}]", (size_t)packet.id);
		auto& manager = engine.getNetManager();
		entt::entity internal_id = engine.getPlayer();
		manager.addServerIDMap(packet.id, internal_id);

	}
}

void NetworkSystem::onSyncSimulationClock(Engine& engine, std::string_view data) {
	Net::CB_SyncSimulationClockPacket packet;
	if (Net::deserializePacketData(data, packet)) {
	}
}

void NetworkSystem::onEntityPositionVel(Engine& engine, std::string_view data) {
	Net::CB_EntityPositionRotPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& manager = engine.getNetManager();

		entt::entity entity = manager.getClientID(packet.entity);
		auto& transform = registry.get<Transformation>(entity);
		transform.pos = packet.pos;	
		transform.rot = packet.rot;
	}
}

void NetworkSystem::onEntitySnapshotDelta(Engine& engine, std::string_view data) {
	
	Net::CB_EntitySnapshotDeltaPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& manager = engine.getNetManager();
		entt::entity player = engine.getPlayer();
		simulation_duration client_time = engine.getClientSimulationTime();


		if (manager.ackEntityStateDelta(packet.server_time) == false) {
			return;
		}
		
		manager.updateAllEntityInterpolation(client_time);


		for (const Net::EntityStateDelta& delta : packet.delta) {
			using namespace Net;

			entt::entity clientID = manager.getClientID(delta.entity);
			if (clientID == player) {
				continue;
			}
			if (clientID == entt::null) {
				using namespace entt;

				clientID = registry.create();
				registry.emplace<Transformation>(clientID, glm::vec3{0});
				registry.emplace<Velocity>(clientID, glm::vec3{0});
				registry.emplace<Renderable>(clientID, "cube"_hs);
				registry.emplace<MovementState>(clientID);
				registry.emplace<Direction>(clientID);
				registry.emplace<VoxelCollider>(clientID, Collision::AABB{ glm::vec3(-.3, -1.5, -.3), glm::vec3(.3, .5, .3) });
				registry.emplace<Network>(clientID);
				manager.addServerIDMap(delta.entity, clientID);
			}

			entt::entity entity = manager.getClientID(delta.entity);
			Net::PositionNetValues* value = manager.insertPositionNetValue(entity, client_time);

			if (delta.hasField(EntityStateDelta::Field::Position)) {
				value->pos = delta.state.pos;
			}
			if (delta.hasField(EntityStateDelta::Field::Rot)) {
				value->rot = delta.state.rot;
			}
			if (delta.hasField(EntityStateDelta::Field::Deleted)) {
				registry.destroy(clientID);
				manager.removeServerIDMap(delta.entity);
				spdlog::info("Goodbye {}", delta.entity);
			}
		}

	}
}

void NetworkSystem::onPlayerPositionAck(Engine& engine, std::string_view data) {
	Net::CB_PlayerPositionAckPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& manager = engine.getNetManager();

		entt::entity player = engine.getPlayer();
		if (manager.ackRequest(packet.pos, packet.client_time) == false) {
			glm::vec3 last_pos = packet.pos;
			for (Net::Request& request : manager.getUnackedRequests()) {
				Transformation transform = request.transform;
				transform.pos = last_pos;
				auto [new_transform, vel] = DVZ::predict_client_player_state(engine, request.input, transform);
				last_pos = new_transform.pos;
				request.transform.pos = last_pos;

			}
			auto& player_transform = registry.get<Transformation>(player);
			spdlog::info("Mispredict by: [{}]", glm::distance(player_transform.pos, last_pos));
			player_transform.pos = last_pos;
		}
	}
}

void NetworkSystem::onPlayerJoin(Engine& engine, std::string_view data) {
	Net::CB_PlayerJoinPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		using namespace entt;
		
		spdlog::info("Player Joined: [{}]", packet.server_id);


		auto& registry = engine.getRegistry();
		auto& manager = engine.getNetManager();
		
		entt::entity clientID = registry.create();
		registry.emplace<Transformation>(clientID, glm::vec3{ 0 });
		registry.emplace<Velocity>(clientID, glm::vec3{0});
		registry.emplace<Direction>(clientID);
		registry.emplace<VoxelCollider>(clientID, Collision::AABB{ glm::vec3(-.3, -1.5, -.3), glm::vec3(.3, .5, .3) });
		registry.emplace<Renderable>(clientID, "cube"_hs);
		//registry.emplace<InterpolateNetValues>(clientID);

		manager.addServerIDMap(packet.server_id, clientID);
	}
}

void NetworkSystem::onSpawnPosition(Engine& engine, std::string_view data) {
	Net::CB_SpawnPositionPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		entt::entity player = engine.getPlayer();
		auto& transform = registry.get<Transformation>(player);
		transform.pos = packet.pos;
	}
}

void NetworkSystem::onChunkData(Engine& engine, std::string_view data) {
	Net::CB_ChunkData packet;
	if (Net::deserializePacketData(data, packet)) {
		Voxel::ClientChunkManager& manager = engine.getChunkManager();
		manager.addCompressedChunk(std::move(packet.compressed));
	}
}
