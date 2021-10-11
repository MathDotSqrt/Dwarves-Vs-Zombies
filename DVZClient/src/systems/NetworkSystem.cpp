#include "client/systems/NetworkSystem.hpp"
#include "client/engine.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

NetworkSystem::NetworkSystem() : netclient(std::make_unique<Net::ClientSocket>()){
	netclient->connectToServer("127.0.0.1:50150");
	assert(netclient->isValid());
}

void NetworkSystem::init(Engine& engine) {

}

void NetworkSystem::gameTick(Engine& engine) {



	if (netclient->isValid()) {
		netclient->pollIncommingMessages();
		if (netclient->getConnectionState() == Net::ConnectionState::CONNECTION_FAILED) {
			spdlog::info("Lets try that again");
			netclient->connectToServer("54.85.202.121:50150");
		}
		if (netclient->getConnectionState() == Net::ConnectionState::CONNECTED) {
			
		}
	}
}