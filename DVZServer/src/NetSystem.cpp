#include "server/NetSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/Engine.hpp"

#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ;

NetSystem::NetSystem(Engine& engine) {

}

void NetSystem::tick(Engine& engine) {
	Net::NetServerManager& netManager = engine.getNetManager();
	netManager.poll([&](std::string_view data, HSteamNetConnection connection) {
		using namespace Net;

		if (data.size() == 0) {
			spdlog::error("Empty message from [{}]", connection);
			return;
		}

		PacketID id{ static_cast<PacketID>(data[0]) };

		data.remove_prefix(1);

		switch (id) {
		case PacketID::ClientConnected:
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
	});
}

void NetSystem::onEchoPacket(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	using namespace Net;

	Net::NetServerManager& netManager = engine.getNetManager();


	EchoPacketData echo;
	deserializePacketData(data, echo);
	spdlog::info("Echo: {}", echo.message);

	netManager.sendMessage(echo, conn);
	netManager.sendMessage(NetPlayerSpawned{ entt::entity{0} }, conn);

	//socket->sendMessage(echo, conn);
	//socket->sendMessage(NetPlayerSpawned{ entt::entity{0} }, conn);
}