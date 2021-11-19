#include "server/net/NetServerManager.hpp"
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

ServerSocket& NetServerManager::getSocket() {
	return *socket;
}

