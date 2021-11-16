#include "server/NetSystem.hpp"
#include "server/net/ServerSocket.hpp"
#include "server/Engine.hpp"

#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ;

NetSystem::NetSystem(Engine& engine) : socket(std::make_unique<Net::ServerSocket>(50150)){

}

void NetSystem::tick(Engine& engine) {
	socket->pollIncomingMessages([&](std::string_view data, HSteamNetConnection connection) {
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
			onEchoPacket(data, connection);
			break;
		default:
			spdlog::error("Message with invalid ID [{}] from [{}]", static_cast<std::byte>(data[0]), connection);
		}

		//socket->sendMessage(data, connection);
	});
}

void NetSystem::onEchoPacket(std::string_view data, HSteamNetConnection conn) {
	using namespace Net;

	EchoPacketData echo;
	deserializePacketData(data, echo);
	spdlog::info("Echo: {}", echo.message);

	socket->sendMessage(echo, conn);
	socket->sendMessage(NetPlayerSpawned{ entt::entity{0} }, conn);
}