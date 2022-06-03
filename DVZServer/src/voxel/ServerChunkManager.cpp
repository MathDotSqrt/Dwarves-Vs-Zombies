#include "server/voxel/ServerChunkManager.hpp"

#include "core/util/util.hpp"
#include "core/util/SpiralIter.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <memory>
#include <spdlog/spdlog.h>

using namespace DVZ;
using namespace DVZ::Voxel;

ServerChunkManager::ServerChunkManager() {
	spdlog::info("Generating world...");
	for (ChunkIndex cx = -WORLD_RADIUS; cx <= WORLD_RADIUS; cx++) {
		for (ChunkIndex cz = -WORLD_RADIUS; cz <= WORLD_RADIUS; cz++) {
			//chunks[ChunkCoords{ cx, 0, cz }] = Chunk{ cx, 0, cz };
			const auto [iter, _] = chunks.emplace(ChunkCoords{cx, 0, cz}, IChunk{ChunkCoords{cx, 0, cz}});
			iter->second.generate();
		}
	}
	spdlog::info("Done.");

}

BlockType ServerChunkManager::getBlock(const WorldCoords& coords) const {
	ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
	const IChunk* chunk = getChunk(chunk_coords);
	if (chunk == nullptr) {
		return BlockType::AIR;
	}

	return chunk->getBlock(Voxel::toBlockCoords(coords));
}

bool ServerChunkManager::setBlock(const WorldCoords& coords, BlockType type) {
	ChunkCoords chunk_coords = toChunkCoords(coords);
	IChunk* chunk = getChunk(chunk_coords);
	if (chunk != nullptr) {
		BlockCoords block_coords = toBlockCoords(coords);
		if (chunk->setBlock(block_coords, type)) {
			compressedChunksCache.erase(chunk_coords);
			return true;
		}
	}

	return false;
}

IChunk* ServerChunkManager::getChunk(const ChunkCoords& coords) {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		return nullptr;
	}
}

const IChunk* ServerChunkManager::getChunk(const ChunkCoords& coords) const {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		return nullptr;
	}
}

void ServerChunkManager::addPlayer(entt::entity id, const glm::vec3& world_pos) {
	playerChunks[id];
}

void ServerChunkManager::removePlayer(entt::entity id) {
	playerChunks.erase(id);
}

void ServerChunkManager::ackChunk(entt::entity id, ChunkCoords coord, int ack) {
	const IChunk* chunk_ptr = getChunk(coord);
	if (chunk_ptr) {
		auto& oldAckNumber = playerChunks.at(id).chunkAckMap[chunk_ptr];
		if (oldAckNumber < ack) {
			oldAckNumber = ack;
		}
	}
}

std::vector<ChunkCoords> ServerChunkManager::getUnackedChunks(entt::entity id, const glm::vec3& world_pos, int N) const {
	std::vector<ChunkCoords> unackedChunks;

	if (N <= 0) {
		return unackedChunks;
	}

	const NetChunkState& state = playerChunks.at(id);
	
	ChunkCoords center = toChunkCoords(world_pos);
	SpiralRange<ChunkIndex> range{center, RENDER_RADIUS};
	for (const auto& chunkCoord : range) {

		const IChunk* chunk_ptr = getChunk(chunkCoord);
		if (chunk_ptr) {
			if (find_with_default(state.chunkAckMap, chunk_ptr, 0) < chunk_ptr->getUpdateCount()) {
				unackedChunks.push_back(chunkCoord);
				if (unackedChunks.size() == N) {
					break;
				}
			}
		}
	}

	return unackedChunks;
}

const CompressedChunk* ServerChunkManager::getCompressedChunk(const ChunkCoords& coords) {
	const auto iter = compressedChunksCache.find(coords);
	if (iter != compressedChunksCache.end()) {
		return &(iter->second);
	}
	
	const IChunk* chunk_ptr = getChunk(coords);
	if (chunk_ptr) {
		const auto [iter, _] = compressedChunksCache.emplace(coords, chunk_ptr->compress());
		return &(iter->second);
	}
	
	return nullptr;
}