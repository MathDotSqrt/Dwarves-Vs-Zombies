#include "client/voxel/ClientChunkManager.hpp"
#include "core/util/math.hpp"

#include <glm/gtx/component_wise.hpp>
#include <spdlog/spdlog.h>

using namespace DVZ::Voxel;

ClientChunkManager::ClientChunkManager() {
	playerChunkCoords = glm::vec3{0};

	for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
		for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
			chunks[ChunkCoords{ cx, 0, cz }] = Chunk{cx, 0, cz};
		}
	}


	ChunkCoords minChunk{-RENDER_RADIUS, 0, -RENDER_RADIUS};
	ChunkCoords maxChunk{ RENDER_RADIUS, 0, RENDER_RADIUS };
	
	minBound = Voxel::toWorldCoords(minChunk, MIN_BLOCK_COORDS);
	maxBound = Voxel::toWorldCoords(maxChunk, MAX_BLOCK_COORDS);
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

Chunk* ClientChunkManager::getChunk(const ChunkCoords& coords) {
	auto iter = chunks.find(coords);
	if (iter != chunks.end()) {
		return &(iter->second);
	}
	else {
		return nullptr;
	}
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

BlockType ClientChunkManager::getBlock(const WorldCoords& coords) const {
	ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
	const Chunk* chunk = getChunk(chunk_coords);
	if (chunk == nullptr) {
		return BlockType::AIR;
	}
	
	return chunk->getBlock(Voxel::toBlockCoords(coords));
}

bool ClientChunkManager::setBlock(const WorldCoords& coords, BlockType block) {

	const auto update_chunk = [this](const WorldCoords& coords) {
		ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
		Chunk* chunk = getChunk(chunk_coords);
		if (chunk) {
			chunk->incrementUpdateCount();
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

float intbound(float s, float ds) {
	if (ds < 0) {
		s = -s;
		ds = -ds;
	}
	s = std::fmodf(s, 1.0f);
	return (1 - s) / ds;
}

std::optional<VoxelRaycastResult> ClientChunkManager::raycast(const glm::vec3& origin, const glm::vec3& dir, float radius) const {
	constexpr float epsilon = 0.001f;

	float dir_length = glm::length(dir);

	if (dir_length < epsilon) {
		return {};
	}

	const glm::ivec3 step = glm::sign(dir);

	const glm::vec3 delta = glm::vec3(step) / dir;

	radius /= dir_length;

	const auto not_past_bounds = [&](const WorldCoords& coords) -> bool {
		return
			(step.x > 0 ? coords.x < maxBound.x : coords.x >= minBound.x) &&
			(step.y > 0 ? coords.y < maxBound.y : coords.y >= minBound.y) &&
			(step.z > 0 ? coords.z < maxBound.z : coords.z >= minBound.z);
	};

	glm::vec3 tmax{ intbound(origin.x, dir.x), intbound(origin.y, dir.y), intbound(origin.z, dir.z) };
	WorldCoords coords = Voxel::toWorldCoords(origin);
	glm::vec3 normal{ 0 };
	
	while (not_past_bounds(coords)) {
		const BlockType type = getBlock(coords);
		if (type != BlockType::AIR) {
			return VoxelRaycastResult{coords, normal, type};
		}

		const auto update_component = [&](int component) -> bool{
			if (tmax[component] > radius) {
				return true;
			}

			coords[component] += step[component];
			tmax[component] += delta[component];
			normal = glm::vec3{ 0 };
			normal[component] = (float)(-step[component]);

			return false;
		};
		
		const auto index = DVZ::compMinIndex(tmax);
		if (update_component(index)) {
			break;
		}	
	}

	return {};
}

bool ClientChunkManager::setBlockInternal(const WorldCoords& coords, BlockType block) {
	ChunkCoords chunk_coords = Voxel::toChunkCoords(coords);
	Chunk* chunk = getChunk(chunk_coords);
	if (chunk == nullptr) {
		return false;
	}

	BlockCoords block_coords = Voxel::toBlockCoords(coords);
	chunk->setBlock(block_coords, block);
	return true;
}

void ClientChunkManager::addCompressedChunk(CompressedChunk&& new_compressed_chunk) {
	auto& compressed_chunk = compressedChunks[new_compressed_chunk.coords];
	if (compressed_chunk.updateCount < new_compressed_chunk.updateCount) {
		compressed_chunk = std::move(new_compressed_chunk);
	}
}

void ClientChunkManager::decompressChunks() {
	for (const auto& [coords, compressed] : compressedChunks) {
		Chunk* chunk = getChunk(coords);
		if (chunk->getUpdateCount() < compressed.updateCount) {
			if (!chunk->decompressChunk(compressed)) {
				spdlog::warn("Failed to decompress chunk: <{},{},{}>", coords.x, coords.y, coords.z);
			}
		}
	}

	compressedChunks.clear();
}

Chunk ClientChunkManager::allocateChunk(const ChunkCoords& coords) {
	if (chunksPool.size() == 0) {
		return Chunk{ coords };
	}
	Chunk chunk = std::move(chunksPool.back());
	chunksPool.pop_back();
	return chunk;
}

void ClientChunkManager::deallocateChunk(Chunk&& chunk) {
	chunksPool.emplace_back(std::move(chunk));
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
			ChunkCoords coords = playerChunkCoords + ChunkCoords{ cx, 0, cz };
			auto iter = chunks.find(coords);
			if (iter == chunks.end()) {
				if (chunksPool.size() > 0) {
					//TODO: generate chunk via thread
					chunksPool.back().init(coords);
					chunks.emplace(coords, std::move(chunksPool.back()));
					chunksPool.pop_back();
				}
				else {
					chunks.emplace(coords, coords);
				}
			}
		}
	}
}