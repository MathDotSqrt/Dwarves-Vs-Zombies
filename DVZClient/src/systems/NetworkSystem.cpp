#include "client/systems/NetworkSystem.hpp"
#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/net/NetClientManager.hpp"


#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

NetworkSystem::NetworkSystem() {
	
}

void NetworkSystem::init(Engine& engine) {

}

void NetworkSystem::gameTick(Engine& engine) {
	using namespace Net;

	Net::NetClientManager& netManager = engine.getNetManager();

	netManager.poll([&](std::string_view sv) {
		this->onMessage(engine, sv);
	});

	auto& registry = engine.getRegistry();
	entt::entity entity = engine.getPlayer();
	
	auto& transform = registry.get<Transformation>(entity);
	auto& network = registry.get<Network>(entity);

	if (glm::distance2(network.last_pos, transform.pos) > .01f || transform.rot != network.last_rot) {
		netManager.sendMessage(Net::SB_PlayerPositionRotPacket{ transform.pos, transform.rot });
		network.last_pos = transform.pos;
		network.last_rot = transform.rot;
	}


	auto view = registry.view<Transformation, InterpolateNetValues>();

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
	case PacketID::CB_EntityPositionVel:
		onEntityPositionVel(engine, data);
		break;
	case PacketID::CB_PlayerJoin:
		onPlayerJoin(engine, data);
		break;
	case PacketID::CB_SpawnPosition:
		onSpawnPosition(engine, data);
		break;
	default:
		break;
	}
}

void NetworkSystem::onAssignNetID(Engine& engine, std::string_view data) {
	Net::CB_AssignNetIDPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& manager = engine.getNetManager();
		entt::entity internal_id = engine.getPlayer();
		manager.addServerIDMap(packet.id, internal_id);
	}
}

void NetworkSystem::onEntityPositionVel(Engine& engine, std::string_view data) {
	Net::CB_EntityPositionRotPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& manager = engine.getNetManager();

		entt::entity server_id = packet.entity;
		entt::entity client_id = manager.getClientID(server_id);

		if (client_id != entt::null) {
			auto& interpolate = registry.get<InterpolateNetValues>(client_id);
			//auto& vel = registry.get<Velocity>(client_id);
			interpolate.pos = packet.pos;
			interpolate.rot = packet.rot;
			interpolate.duration_seconds = 0;
			//vel = packet.vel;
		}
	}
}

void NetworkSystem::onPlayerJoin(Engine& engine, std::string_view data) {
	Net::CB_PlayerJoinPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		using namespace entt;
		
		auto& registry = engine.getRegistry();
		auto& manager = engine.getNetManager();
		
		entt::entity clientID = registry.create();
		registry.emplace<Transformation>(clientID, glm::vec3{ 0 });
		registry.emplace<Velocity>(clientID, glm::vec3{0});
		registry.emplace<Direction>(clientID);
		registry.emplace<VoxelCollider>(clientID, Collision::AABB{ glm::vec3(-.3, -1.5, -.3), glm::vec3(.3, .5, .3) });
		registry.emplace<Renderable>(clientID, "cube"_hs);
		registry.emplace<InterpolateNetValues>(clientID);

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