#include "server/net/NetServerManager.hpp"
#include "core/time.hpp"
#include <spdlog/spdlog.h>

using namespace DVZ;
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
		auto& [last_client_time, updated] = iter->second;
		if (last_client_time < client_time) {
			last_client_time = client_time;
			updated = true;
			return true;
		}
		updated = false;
		return false;
	}

	entityLastUpdateTime[entity] = std::make_pair( client_time, true);
	return true;
}

std::optional<simulation_duration> NetServerManager::shouldAckEntityInput(entt::entity entity) {
	const auto iter = entityLastUpdateTime.find(entity);
	if(iter != entityLastUpdateTime.end()){
		auto& [client_time, updated] = iter->second;
		if (updated) {
			updated = false;
			return client_time;
		}
	}
	return {};
}