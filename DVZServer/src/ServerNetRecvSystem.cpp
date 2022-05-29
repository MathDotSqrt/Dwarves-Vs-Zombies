#include "server/ServerNetRecvSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/Engine.hpp"
#include "server/ServerComponents.hpp"
#include "server/voxel/ServerChunkManager.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"
#include "core/util/Timer.hpp"

#include <glm/gtx/norm.hpp>
#include <spdlog/spdlog.h>

using namespace DVZ;

ServerNetRecvSystem::ServerNetRecvSystem() {

}

void ServerNetRecvSystem::init(Engine& engine){
	auto& registry = engine.getRegistry();

	entt::entity test = registry.create();
	registry.emplace<Transformation>(test, glm::vec3{ 0, 100, 0 });
	registry.emplace<Velocity>(test);
	registry.emplace<Debug>(test, 0.0f);
	registry.emplace<Network>(test);
}

void ServerNetRecvSystem::tick(Engine& engine) {
	using namespace Net;

	Timer timer{"ServerNetRecvSystem::tick"};

	Net::NetServerManager& netManager = engine.getNetManager();
	netManager.poll([&](std::string_view data, HSteamNetConnection connection) {
		onMessage(engine, data, connection);
	});

}

void ServerNetRecvSystem::onMessage(Engine& engine, std::string_view data, HSteamNetConnection connection) {
	using namespace Net;

	if (data.size() == 0) {
		spdlog::error("Empty message from [{}]", connection);
		return;
	}

	PacketID id{ static_cast<PacketID>(data[0]) };

	data.remove_prefix(1);

	switch (id) {
	case PacketID::SB_ClientJoin:
		onClientJoin(engine, data, connection);
		break;
	case PacketID::SB_ClientDisconnected:
		onClientDisconnected(engine, data, connection);
		break;
	case PacketID::SB_PlayerPositionVel:
		onPlayerPositionVel(engine, data, connection);
		break;
	case PacketID::SB_PlayerInput:
		onPlayerInput(engine, data, connection);
		break;
	case PacketID::SB_AckEntitySnapshotDelta:
		onAckEntityStateDelta(engine, data, connection);
		break;
	case PacketID::SB_AckChunkData:
		onAckChunkData(engine, data, connection);
		break;
	default:
		spdlog::error("Message with invalid ID [{}] from [{}]", static_cast<std::byte>(data[0]), connection);
		break;
	}
}

void ServerNetRecvSystem::onClientJoin(Engine& engine, std::string_view data, HSteamNetConnection conn){
	Net::SB_ClientJoinPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		spdlog::info("Welcome: {}", packet.name);
		auto& registry = engine.getRegistry();
		auto& netManager = engine.getNetManager();
		auto& chunkManager = engine.getChunkManager();

		entt::entity player = registry.create();

		glm::vec3 spawn_pos{0, 100, 0};
		registry.emplace<Transformation>(player, spawn_pos);
		registry.emplace<Velocity>(player, glm::vec3{0});
		registry.emplace<MovementState>(player);
		registry.emplace<Direction>(player);
		registry.emplace<VoxelCollider>(player, Collision::AABB{ glm::vec3(-.3, -1.5, -.3), glm::vec3(.3, .5, .3) });
		registry.emplace<Network>(player);
		registry.emplace<NetPlayer>(player);

		chunkManager.addPlayer(player, spawn_pos);

		netManager.addEntityConnectionMapping(player, conn);
		netManager.sendMessage(Net::CB_AssignNetIDPacket{player}, conn, true);
		netManager.sendMessage(Net::CB_SpawnPositionPacket{spawn_pos}, conn, true);
	}
}

void ServerNetRecvSystem::onClientDisconnected(Engine& engine, std::string_view, HSteamNetConnection conn) {
	spdlog::info("Good Bye!");
	auto& registry = engine.getRegistry();
	auto& netManager = engine.getNetManager();
	auto& chunkManager = engine.getChunkManager();

	entt::entity player = netManager.getEntity(conn);

	netManager.removeEntityConnectionMapping(conn);
	chunkManager.removePlayer(player);
	registry.destroy(player);
}

void ServerNetRecvSystem::onPlayerPositionVel(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_PlayerPositionRotPacket packet;
	if (Net::deserializePacketData(data, packet)) {

	}
}

void ServerNetRecvSystem::onPlayerInput(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_PlayerInput packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& netManager = engine.getNetManager();
		entt::entity player = netManager.getEntity(conn);
		
		if (netManager.setEntityInputTime(player, packet.client_time)) {
			auto& transform = registry.get<Transformation>(player);
			auto& state = registry.get<MovementState>(player);

			transform.rot = packet.rot;
			state.forward = packet.getForward();
			state.strafe = packet.getStrafe();
			state.fly = packet.getFly();
		}
	}
}

void ServerNetRecvSystem::onAckEntityStateDelta(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_AckEntitySnapshoDelta packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& manager = engine.getNetManager();
		manager.ackClientSnapshot(conn, packet.server_time);
	}
}

void ServerNetRecvSystem::onAckChunkData(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_AckChunkData packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& netManager = engine.getNetManager();
		auto& chunkManager = engine.getChunkManager();
		auto& registry = engine.getRegistry();

		entt::entity playerID = netManager.getEntity(conn);
		chunkManager.ackChunk(playerID, packet.coords, packet.updateCount);

		auto& netPlayer = registry.get<NetPlayer>(playerID);
		const auto iter = std::find(netPlayer.unackedChunks.begin(), netPlayer.unackedChunks.end(), packet.coords);
		netPlayer.unackedChunks.erase(iter);
	}
}
