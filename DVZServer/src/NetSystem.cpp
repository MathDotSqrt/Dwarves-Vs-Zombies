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

		PlayerPositionVelPacketData data;
		data.entity = entity;
		data.pos = trans.pos;
		data.vel = vel;

		netManager.sendToAllMessage(data);
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
	case PacketID::ClientConnected:
		onClientConnected(engine, data, connection);
		break;
	case PacketID::NetPlayerSpawned:
		break;
	case PacketID::PlayerPositionVel:
		break;
	case PacketID::Echo:
		onEchoPacket(engine, data, connection);
		break;
	default:
		spdlog::error("Message with invalid ID [{}] from [{}]", static_cast<std::byte>(data[0]), connection);
		break;
	}
}

void NetSystem::onClientConnected(Engine& engine, std::string_view data, HSteamNetConnection conn){
	using namespace Net;

	ClientConnectedPacketData client;
	deserializePacketData(data, client);
	spdlog::info("Welcome: {}", client.name);

	auto& registry = engine.getRegistry();
	auto& netManager = engine.getNetManager();

	entt::entity entity = registry.create();
	registry.emplace<Transformation>(entity, glm::vec3{0, 20, -10});
	registry.emplace<Velocity>(entity, glm::vec3{0, .1f, 0});
	//registry.emplace<NetPlayer>(newclient, conn);

	netManager.sendMessage(NetPlayerSpawned{ entity }, conn);
}

void NetSystem::onEchoPacket(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	using namespace Net;

	Net::NetServerManager& netManager = engine.getNetManager();


	EchoPacketData echo;
	deserializePacketData(data, echo);
	spdlog::info("Echo: {}", echo.message);
	netManager.sendMessage(echo, conn);
}