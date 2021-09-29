#include "client/voxel/ClientChunkManager.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Voxel;

ClientChunkManager::ClientChunkManager() {
	playerChunkCoords = glm::vec3{0};

	for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
		for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
			chunks.emplace_back(cx, 0, cz);
		}
	}

}

void ClientChunkManager::updatePlayerPosition(const glm::vec3& playerCoords) {
	ChunkCoords newChunkCoords = Voxel::toChunkCoords(playerCoords);
	hasChanged = newChunkCoords != playerChunkCoords;
	playerChunkCoords = newChunkCoords;

	if (hasChanged) {
		spdlog::debug("Player Chunk Coord <{}, {}, {}>", playerChunkCoords.x, playerChunkCoords.y, playerChunkCoords.z);
	}
}

const ChunkCoords& ClientChunkManager::getPlayerChunkCoords() const {
	return playerChunkCoords;
}

const std::vector<Chunk>& ClientChunkManager::getChunks() const {
	return chunks;
}