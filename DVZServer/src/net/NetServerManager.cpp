#include "server/net/NetServerManager.hpp"
#include "core/time.hpp"
#include <spdlog/spdlog.h>

using namespace DVZ;
using namespace DVZ::Net;

NetServerManager::NetServerManager() : socket(std::make_unique<Net::ServerSocket>(50150)) {
	
} 

void NetServerManager::update() {
	
}

void NetServerManager::addEntityConnectionMapping(entt::entity entity, HSteamNetConnection connection) {
	entityToConnectionMap[entity] = connection;
	connectionToEntityMap[connection] = entity;
	clientSnapshots[connection];
}

void NetServerManager::removeEntityConnectionMapping(HSteamNetConnection conn) {
	size_t num_removed = connectionToEntityMap.erase(conn);
	clientSnapshots.erase(conn);
	if (num_removed > 0) {
		entt::entity entity = getEntity(conn);
		entityToConnectionMap.erase(entity);
	}
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
		//if (last_client_time < client_time) {
		//	last_client_time = client_time;
		//	updated = true;
		//	return true;
		//}
		updated = true;
		return true;
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

void NetServerManager::setEntitySnapshot(std::shared_ptr<EntitySnapshot> gamestate) {
	this->masterGamestate = std::move(gamestate);

	for (auto& [connection, gamestateBuffer] : clientSnapshots) {
		gamestateBuffer.push_back(this->masterGamestate);
	}
}

void NetServerManager::ackClientSnapshot(HSteamNetConnection client, simulation_duration ack_simulation_time) {
	const auto iter = clientSnapshots.find(client);
	if (iter != clientSnapshots.end()) {
		auto& snapshotBuffer = iter->second;
		const auto find_iter = std::find_if(snapshotBuffer.begin(), snapshotBuffer.end(), [&](const std::shared_ptr<EntitySnapshot>& snapshot) {
			return snapshot->getSimulationTime() == ack_simulation_time;
		});

		if (find_iter != snapshotBuffer.end()) {
			//Makes first element the last acked snapshot
			snapshotBuffer.erase(snapshotBuffer.begin(), find_iter);
		}
	}
}

EntitySnapshotDelta NetServerManager::getClientSnapshotDelta(HSteamNetConnection client) const {
	const auto iter = clientSnapshots.find(client);
	if (iter != clientSnapshots.end()) {
		const auto& snapshotBuffer = iter->second;
		if (snapshotBuffer.size() == 1) {
			return EntitySnapshot::computeDelta(zeroSnapshot, *snapshotBuffer.front());
		}
		else {
			return EntitySnapshot::computeDelta(*snapshotBuffer.front(), *snapshotBuffer.back());
		}
	}

	assert(false);
	return {};
}

const EntitySnapshot NetServerManager::zeroSnapshot{};
