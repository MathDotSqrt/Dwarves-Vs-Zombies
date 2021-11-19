#include "server/NetSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/Engine.hpp"
#include "server/ServerComponents.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ;

NetSystem::NetSystem(Engine& engine) {

}

void NetSystem::tick(Engine& engine) {
	using namespace Net;

	Net::NetServerManager& netManager = engine.getNetManager();
	netManager.poll([&](std::string_view data, HSteamNetConnection connection) {
		onMessage(engine, data, connection);
	});

	auto& registry = engine.getRegistry();

	auto view = registry.view<Transformation, Velocity>();

	for (entt::entity entity : view) {
		auto& trans = view.get<Transformation>(entity);
		const auto& vel = view.get<Velocity>(entity);

		Net::CB_EntityPositionVelPacket data;
		data.entity = entity;
		data.pos = trans.pos;
		data.vel = vel;

		netManager.sendToAllMessage(data, netManager.getConnectionHandle(entity));
		trans.pos = trans.pos + vel;

	}
}

void NetSystem::onMessage(Engine& engine, std::string_view data, HSteamNetConnection connection) {
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
	default:
		spdlog::error("Message with invalid ID [{}] from [{}]", static_cast<std::byte>(data[0]), connection);
		break;
	}
}

void NetSystem::onClientJoin(Engine& engine, std::string_view data, HSteamNetConnection conn){
	Net::SB_ClientJoinPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		spdlog::info("Welcome: {}", packet.name);

		auto& registry = engine.getRegistry();
		auto& netManager = engine.getNetManager();

		entt::entity clientID = registry.create();
		spdlog::info("New clientID [{}]", clientID);

		glm::vec3 spawn_pos = glm::vec3(0, 100, 0);

		registry.emplace<Transformation>(clientID, spawn_pos);
		registry.emplace<Velocity>(clientID, glm::vec3{0});

		netManager.addEntityConnectionMapping(clientID, conn);

		netManager.sendMessage(Net::CB_AssignNetIDPacket{clientID}, conn);
		netManager.sendMessage(Net::CB_SpawnPositionPacket{ spawn_pos }, conn);
		netManager.sendToAllMessage(Net::CB_PlayerJoinPacket{ clientID }, conn);
	}
}

void NetSystem::onPlayerPositionVel(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_PlayerPositionVel packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& netManager = engine.getNetManager();
		entt::entity player = netManager.getEntity(conn);
		auto& transform = registry.get<Transformation>(player);
		
		transform.pos = packet.pos;
	}
}

