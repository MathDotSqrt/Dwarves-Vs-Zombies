#include "client/net/NetClientManager.hpp"

#include <spdlog/spdlog.h>
#include <assert.h>

using namespace DVZ::Net;
NetClientManager::NetClientManager() : netclient(std::make_unique<Net::ClientSocket>()) {
	netclient->connectToServer("127.0.0.1:50150");
	assert(netclient->isValid());
}

void NetClientManager::update() {
	static bool sent = false;

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
