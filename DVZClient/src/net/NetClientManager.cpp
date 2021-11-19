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
			netclient->sendMessage(SB_ClientJoinPacket{"MathDotSqrt"});
			sent = true;
		}
	}
}

bool NetClientManager::hasServerID(entt::entity server) const {
	return serverIDMap.find(server) != serverIDMap.end();
}

void NetClientManager::addServerIDMap(entt::entity server, entt::entity client) {
	serverIDMap[server] = client;
}

entt::entity NetClientManager::getClientID(entt::entity server) const {
	const auto iter = serverIDMap.find(server);
	if (iter != serverIDMap.end()) {
		return iter->second;
	}

	return entt::null;
}
