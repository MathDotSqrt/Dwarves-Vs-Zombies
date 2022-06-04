#include "client/voxel/ClientChunkManager.hpp"
#include "core/util/math.hpp"

#include <glm/gtx/component_wise.hpp>
#include <spdlog/spdlog.h>

using namespace DVZ::Voxel;

ClientChunkManager::ClientChunkManager() {
	playerChunkCoords = glm::vec3{0};

	//for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
	//	for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
	//		chunks[ChunkCoords{ cx, 0, cz }] = Chunk{cx, 0, cz};
	//	}
	//}

	ChunkCoords minChunk{-RENDER_RADIUS, 0, -RENDER_RADIUS};
	ChunkCoords maxChunk{ RENDER_RADIUS, 0, RENDER_RADIUS };
	
	minBound = Voxel::toWorldCoords(minChunk, MIN_BLOCK_COORDS);
	maxBound = Voxel::toWorldCoords(maxChunk, MAX_BLOCK_COORDS);
}

BlockType ClientChunkManager::getBlock(const WorldCoords& coords) const {
	ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
	const IChunk* chunk = getChunk(chunk_coords);
	if (chunk == nullptr) {
		return BlockType::AIR;
	}

	return chunk->getBlock(Voxel::toBlockCoords(coords));
}

bool ClientChunkManager::setBlock(const WorldCoords& coords, BlockType block) {

	const auto update_chunk = [this](const WorldCoords& coords) {
		ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
		ClientChunk* chunk = getClientChunk(chunk_coords);
		if (chunk) {
			chunk->incrementRenderUpdateCount();
		}
	};

	if (setBlockInternal(coords, block)) {
		constexpr WorldCoords X{ 1, 0, 0 };
		constexpr WorldCoords Y{ 0, 1, 0 };
		constexpr WorldCoords Z{ 0, 0, 1 };

		ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
		BlockCoords block_coords = Voxel::toBlockCoords(coords);

		if (block_coords.x == MIN_BLOCK_COORDS.x) {
			update_chunk(coords - X);
		}
		else if (block_coords.x == MAX_BLOCK_COORDS.x) {
			update_chunk(coords + X);
		}

		if (block_coords.y == MIN_BLOCK_COORDS.y) {
			update_chunk(coords - Y);
		}
		else if (block_coords.y == MAX_BLOCK_COORDS.y) {
			update_chunk(coords + Y);
		}

		if (block_coords.z == MIN_BLOCK_COORDS.z) {
			update_chunk(coords - Z);
		}
		else if (block_coords.z == MAX_BLOCK_COORDS.z) {
			update_chunk(coords + Z);
		}

		return true;
	}
	return false;
}

const IChunk* ClientChunkManager::getChunk(const ChunkCoords& coords) const {
	return getClientChunk(coords);
}

IChunk* ClientChunkManager::getChunk(const ChunkCoords& coords) {
	return getClientChunk(coords);
}

ClientChunk* ClientChunkManager::getClientChunk(const ChunkCoords& coords) {
	return const_cast<ClientChunk*>(const_cast<const ClientChunkManager*>(this)->getClientChunk(coords));
}

const ClientChunk* ClientChunkManager::getClientChunk(const ChunkCoords& coords) const {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		return nullptr;
	}
}

const ChunkCoords& ClientChunkManager::getPlayerChunkCoords() const {
	return playerChunkCoords;
}

void ClientChunkManager::updatePlayerPosition(const glm::vec3& playerCoords) {
	ChunkCoords newChunkCoords = Voxel::toChunkCoords(playerCoords);
	hasChanged = newChunkCoords != playerChunkCoords;

	if (hasChanged) {
		playerChunkCoords = newChunkCoords;
		spdlog::debug("Player Chunk Coord <{}, {}, {}>", playerChunkCoords.x, playerChunkCoords.y, playerChunkCoords.z);
		//queueChunksToDelete();
		//queueChunksToGenerate();
	}
	decompressChunks();

}

bool ClientChunkManager::setBlockInternal(const WorldCoords& coords, BlockType block) {
	ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
	IChunk* chunk = getChunk(chunk_coords);
	if (chunk == nullptr) {
		return false;
	}

	BlockCoords block_coords = Voxel::toBlockCoords(coords);
	return chunk->setBlock(block_coords, block);
}

bool ClientChunkManager::addCompressedChunk(CompressedChunk&& new_compressed_chunk) {
	auto& compressed_chunk = compressedChunks[new_compressed_chunk.coords];
	if (compressed_chunk.updateCount < new_compressed_chunk.updateCount) {
		compressed_chunk = std::move(new_compressed_chunk);
		return true;
	}
	return false;
}

void ClientChunkManager::decompressChunks() {
	for (const auto& [coords, compressed] : compressedChunks) {
		IChunk* chunk = getChunkOrAllocate(coords);
		if (chunk->getUpdateCount() < compressed.updateCount) {
			if (!chunk->decompress(compressed)) {
				spdlog::warn("Failed to decompress chunk: <{},{},{}>", coords.x, coords.y, coords.z);
			}

			if (ClientChunk* neighbor = getClientChunk(chunk->getChunkCoords() + ChunkCoords{ 1, 0, 0 }); neighbor) {
				neighbor->incrementRenderUpdateCount();
			}
			if (ClientChunk* neighbor = getClientChunk(chunk->getChunkCoords() + ChunkCoords{ -1, 0, 0 }); neighbor) {
				neighbor->incrementRenderUpdateCount();
			}
			if (ClientChunk* neighbor = getClientChunk(chunk->getChunkCoords() + ChunkCoords{ 0, 0, 1 }); neighbor) {
				neighbor->incrementRenderUpdateCount();
			}
			if (ClientChunk* neighbor = getClientChunk(chunk->getChunkCoords() + ChunkCoords{ 0, 0, -1 }); neighbor) {
				neighbor->incrementRenderUpdateCount();
			}
		}
	}

	compressedChunks.clear();
}

IChunk* ClientChunkManager::getChunkOrAllocate(const ChunkCoords& coords) {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		const auto&[iter, _] = chunks.emplace(coords, ClientChunk{coords});
		return &(iter->second);
	}
}