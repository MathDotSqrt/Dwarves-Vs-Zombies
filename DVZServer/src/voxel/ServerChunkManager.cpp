#include "server/voxel/ServerChunkManager.hpp"

using namespace DVZ;
using namespace DVZ::Voxel;

ServerChunkManager::ServerChunkManager() {
	for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
		for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
			chunks[ChunkCoords{ cx, 0, cz }] = Chunk{ cx, 0, cz };
		}
	}
}

const Chunk* ServerChunkManager::getChunk(const ChunkCoords& coords) const {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		return nullptr;
	}
}

BlockType ServerChunkManager::getBlock(const WorldCoords& coords) const {
	ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
	const Chunk* chunk = getChunk(chunk_coords);
	if (chunk == nullptr) {
		return BlockType::AIR;
	}

	return chunk->getBlock(Voxel::toBlockCoords(coords));
}