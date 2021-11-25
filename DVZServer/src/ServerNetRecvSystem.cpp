#include "server/ServerNetRecvSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/Engine.hpp"
#include "server/ServerComponents.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

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
	registry.emplace<NetPlayer>(test);
}

void ServerNetRecvSystem::tick(Engine& engine) {
	using namespace Net;

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
	case PacketID::SB_PlayerPositionVel:
		onPlayerPositionVel(engine, data, connection);
		break;
	case PacketID::SB_PlayerInput:
		onPlayerInput(engine, data, connection);
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
		
		entt::entity player = registry.create();

		netManager.addEntityConnectionMapping(player, conn);

		glm::vec3 spawn_pos{0, 100, 0};
		registry.emplace<Transformation>(player, spawn_pos);
		registry.emplace<Velocity>(player, glm::vec3{0});
		registry.emplace<MovementState>(player);
		registry.emplace<Direction>(player);
		registry.emplace<Network>(player);
		registry.emplace<NetPlayer>(player);

		netManager.sendMessage(Net::CB_AssignNetIDPacket{player}, conn, true);
		netManager.sendMessage(Net::CB_SpawnPositionPacket{spawn_pos}, conn, true);
	}
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
			state.forward = packet.forward;
			state.strafe = packet.strafe;
			state.fly = packet.fly;
		}
	}
}

