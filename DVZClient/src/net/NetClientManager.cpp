#include "client/net/NetClientManager.hpp"

#include <spdlog/spdlog.h>
#include <assert.h>

using namespace DVZ::Net;
NetClientManager::NetClientManager() : netclient(std::make_unique<Net::ClientSocket>()) {
	//netclient->connectToServer("127.0.0.1:50150");
	//assert(netclient->isValid());
}

void NetClientManager::connectTo(std::string_view ip) {
	netclient->connectToServer(std::string{ ip });
}

void NetClientManager::update() {
	static bool sent = false;

	if (netclient->getConnectionState() == ConnectionState::CONNECTION_FAILED) {
		spdlog::info("Lets try that again");
		//netclient->connectToServer("54.85.202.121:50150");
		//netclient->connectToServer("127.0.0.1:50150");
	}
	if (netclient->getConnectionState() == ConnectionState::CONNECTED) {
		if (sent == false) {
			sendMessage(SB_ClientJoinPacket{"MathDotSqrt"});
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

void NetClientManager::appendRequest(const MovementState& input, const glm::vec3& pos, simulation_duration client_time) {
	unackedRequestBuffer.push_back(Request{input, pos, client_time});
}

bool NetClientManager::ackRequest(const glm::vec3& auth_pos, simulation_duration client_request_time) {
	const auto iter = std::find_if(unackedRequestBuffer.begin(), unackedRequestBuffer.end(), [&](const Request& request) {
		return request.client_time == client_request_time;
	});
	
	assert(iter != unackedRequestBuffer.end());

	const glm::vec3& predicted_pos = iter->pos;
	bool is_prediction_correct = glm::distance2(predicted_pos, auth_pos) < .01f;
	unackedRequestBuffer.erase(unackedRequestBuffer.begin(), iter + 1);

	return is_prediction_correct;
	
}