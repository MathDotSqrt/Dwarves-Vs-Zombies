#include "server/net/NetServerManager.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Net;

NetServerManager::NetServerManager() : socket(std::make_unique<Net::ServerSocket>(50150)) {
	
} 

void NetServerManager::addEntityConnectionMapping(entt::entity entity, HSteamNetConnection connection) {
	entityToConnectionMap[entity] = connection;
	connectionToEntityMap[connection] = entity;
}

HSteamNetConnection NetServerManager::getConnectionHandle(entt::entity entity) const {
	const auto iter = entityToConnectionMap.find(entity);

	if (iter != entityToConnectionMap.end()) {
		return iter->second;
	}

	return k_HSteamNetConnection_Invalid;
}

entt::entity NetServerManager::getEntity(HSteamNetConnection connection) const { 
	const auto iter = connectionToEntityMap.find(connection);

	if (iter != connectionToEntityMap.end()) {
		return iter->second;
	}

	return entt::null;
}

bool NetServerManager::setEntityInputTime(entt::entity entity, simulation_duration client_time) {
	const auto iter = entityLastUpdateTime.find(entity);
	if (iter != entityLastUpdateTime.end()) {
		if (iter->second < client_time) {
			iter->second = client_time;
			return true;
		}
		return false;
	}

	entityLastUpdateTime[entity] = client_time;
	return true;
}
