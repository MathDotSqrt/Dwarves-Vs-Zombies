#include "client/voxel/ClientChunkManager.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Voxel;

void ClientChunkManager::updatePlayerPosition(const glm::vec3& playerCoords) {
	ChunkCoords newChunkCoords = Voxel::toChunkCoords(playerCoords);
	hasChanged = newChunkCoords != playerChunkCoords;
	playerChunkCoords = newChunkCoords;

	if (hasChanged) {
		spdlog::debug("Player Chunk Coord <{}, {}, {}>", playerChunkCoords.x, playerChunkCoords.y, playerChunkCoords.z);
	}
}

