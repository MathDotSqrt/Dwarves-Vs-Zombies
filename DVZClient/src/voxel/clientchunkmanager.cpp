#include "client/voxel/ClientChunkManager.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Voxel;

ClientChunkManager::ClientChunkManager() {
	playerChunkCoords = glm::vec3{0};

	for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
		for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
			chunks[ChunkCoords{ cx, 0, cz }] = Chunk{cx, 0, cz};
		}
	}

}

void ClientChunkManager::updatePlayerPosition(const glm::vec3& playerCoords) {
	ChunkCoords newChunkCoords = Voxel::toChunkCoords(playerCoords);
	hasChanged = newChunkCoords != playerChunkCoords;

	if (hasChanged) {
		playerChunkCoords = newChunkCoords;
		spdlog::debug("Player Chunk Coord <{}, {}, {}>", playerChunkCoords.x, playerChunkCoords.y, playerChunkCoords.z);
		queueChunksToDelete();
		queueChunksToGenerate();
	}
}

void ClientChunkManager::queueChunksToDelete() {
	auto iter = chunks.begin();
	const auto end = chunks.end();
	while (iter != end) {
		auto& [coords, chunk] = *iter;
		ChunkCoords delta = glm::abs(playerChunkCoords - coords);
		if (delta.x > RENDER_RADIUS || delta.z > RENDER_RADIUS) {
			chunksPool.emplace_back(std::move(chunk));
			iter = chunks.erase(iter);
		}
		else {
			++iter;
		}
	}
}

void ClientChunkManager::queueChunksToGenerate() {
	for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
		for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
			ChunkCoords coords{ cx, 0, cz };
			auto iter = chunks.find(coords);
			if (iter == chunks.end()) {
				if (chunksPool.size() > 0) {
					chunks.emplace(coords, std::move(chunksPool.back()));
				}
				else {
					chunks.emplace(coords, coords);
				}
			}
		}
	}
}

const Chunk* ClientChunkManager::getChunk(const ChunkCoords& coords) const {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		return nullptr;
	}
}

const Chunk* ClientChunkManager::getChunk(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz) const {
	return getChunk(ChunkCoords{cx, cy, cz});
}

ChunkNeighbors ClientChunkManager::getChunkNeighbors(const ChunkCoords& coords) const {
	constexpr ChunkCoords X{ 1, 0, 0 };
	constexpr ChunkCoords Y{ 0, 1, 0 };
	constexpr ChunkCoords Z{ 0, 0, 1 };
	
	ChunkNeighbors n;
	n.center = getChunk(coords);
	n.nx = getChunk(coords - X);
	n.px = getChunk(coords + X);
	n.ny = getChunk(coords - Y);
	n.py = getChunk(coords + Y);
	n.nz = getChunk(coords - Z);
	n.pz = getChunk(coords + Z);

	return n;
}

const ChunkCoords& ClientChunkManager::getPlayerChunkCoords() const {
	return playerChunkCoords;
}