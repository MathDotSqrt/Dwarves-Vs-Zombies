#include "client/systems/NetworkSystem.hpp"
#include "client/engine.hpp"

#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

NetworkSystem::NetworkSystem() : netclient(std::make_unique<Net::ClientSocket>()){
	netclient->connectToServer("127.0.0.1:50150");
	assert(netclient->isValid());
	this->func = std::bind(&NetworkSystem::onMessage, this, std::placeholders::_1);
}

void NetworkSystem::init(Engine& engine) {
	const auto buffer = DVZ::Net::serializePacketData(Net::PlayerPositionVelPacketData{glm::vec3(0), glm::vec3(1)});
	spdlog::info("Buffer size: {}", buffer.size());
}

void NetworkSystem::gameTick(Engine& engine) {
	using namespace Net;
	static bool sent = false;
	if (netclient->isValid()) {
		//netclient->pollIncommingMessages([&](std::string_view message) {processMessage(message); });
		netclient->pollIncommingMessages(func);


		if (netclient->getConnectionState() == ConnectionState::CONNECTION_FAILED) {
			spdlog::info("Lets try that again");
			netclient->connectToServer("54.85.202.121:50150");
		}
		if (netclient->getConnectionState() == ConnectionState::CONNECTED) {
			if (sent == false) {
				netclient->sendMessage(EchoPacketData{ "test" });
				sent = true;
			}
		}
	}
}

void NetworkSystem::onMessage(std::string_view data) {
	using namespace Net;

	if (data.size() == 0) {
		spdlog::warn("Packet with zero size recieved");
		return;
	}

	PacketID packetID = static_cast<PacketID>(data[0]);
	data.remove_prefix(1);

	switch (packetID) {
	case PacketID::ClientConnected:
		break;
	case PacketID::Echo:
		onEchoPacket(data);
		break;
	case PacketID::PlayerPositionVel:
		onPlayerPosVelPacket(data);
		break;
	default:
		break;
	}
}

void NetworkSystem::onEchoPacket(std::string_view data) {
	using namespace Net;
	EchoPacketData echo;
	if (deserializePacketData(data, echo)) {
		spdlog::info("Echo: {}", echo.message);
	}
}

void NetworkSystem::onPlayerPosVelPacket(std::string_view data) {
	using namespace Net;
	PlayerPositionVelPacketData posvel;
	if (deserializePacketData(data, posvel)) {
		
	}
}