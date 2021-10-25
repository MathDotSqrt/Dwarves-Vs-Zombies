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
	
	if (netclient->isValid()) {
		//netclient->pollIncommingMessages([&](std::string_view message) {processMessage(message); });
		netclient->pollIncommingMessages(func);


		if (netclient->getConnectionState() == Net::ConnectionState::CONNECTION_FAILED) {
			spdlog::info("Lets try that again");
			netclient->connectToServer("54.85.202.121:50150");
		}
		if (netclient->getConnectionState() == Net::ConnectionState::CONNECTED) {
			netclient->sendMessage("Echo!");
		}
	}
}

void NetworkSystem::onMessage(std::string_view message) {
	spdlog::info("{}", message);
}