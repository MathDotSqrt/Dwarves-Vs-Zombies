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
		//spdlog::info("Lets try that again");
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
	entityInterpolationMapBuffer[client];

}

void NetClientManager::removeServerIDMap(entt::entity server) {
	const auto iter = serverIDMap.find(server);
	if (iter != serverIDMap.end()) {
		entt::entity clientID = iter->second;
		entityInterpolationMapBuffer.erase(clientID);
	}
	serverIDMap.erase(server);
}

entt::entity NetClientManager::getClientID(entt::entity server) const {
	const auto iter = serverIDMap.find(server);
	if (iter != serverIDMap.end()) {
		return iter->second;
	}

	return entt::null;
}

void NetClientManager::appendRequest(const MovementState& input, const Transformation& transform, simulation_duration client_time) {
	unackedRequestBuffer.push_back(Request{input, transform, client_time});
}

bool NetClientManager::ackRequest(const glm::vec3& auth_pos, simulation_duration client_request_time) {
	const auto iter = std::find_if(unackedRequestBuffer.begin(), unackedRequestBuffer.end(), [&](const Request& request) {
		return request.client_time == client_request_time;
	});

	if (iter != unackedRequestBuffer.end()) {
		const glm::vec3& predicted_pos = iter->transform.pos;
		bool is_prediction_correct = glm::distance2(predicted_pos, auth_pos) < .01f;

		unackedRequestBuffer.erase(unackedRequestBuffer.begin(), iter + 1);

		return is_prediction_correct;
	}
	return true;
}

std::vector<Request>& NetClientManager::getUnackedRequests() {
	return unackedRequestBuffer;
}

bool NetClientManager::ackEntityStateDelta(simulation_duration server_time) {
	if (server_time > lastAckEntityStateServerTime) {
		lastAckEntityStateServerTime = server_time;
		Net::SB_AckEntitySnapshoDelta ackPacket;
		ackPacket.server_time = server_time;
		sendMessage(ackPacket, false);
		return true;
	}
	return false;
}

void NetClientManager::updateAllEntityInterpolation(simulation_duration client_time) {
	ackEntityStateBuffer.push_back(client_time);
}

PositionNetValues* NetClientManager::insertPositionNetValue(entt::entity clientID, simulation_duration client_time) {
	const auto iter = entityInterpolationMapBuffer.find(clientID);
	if (iter != entityInterpolationMapBuffer.end()) {
		return iter->second.insertPositionNetValues(client_time);
	}
	return nullptr;
}

std::optional<PositionNetValues> NetClientManager::getInterpolatedEntityValues(entt::entity clientID, simulation_duration client_time) const {
	const auto iter = entityInterpolationMapBuffer.find(clientID);
	if (iter != entityInterpolationMapBuffer.end()) {
		return iter->second.computeInterpolation(client_time, ackEntityStateBuffer);
	}
	return {};
}
