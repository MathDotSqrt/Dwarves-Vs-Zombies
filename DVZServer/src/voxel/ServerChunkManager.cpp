#include "server/voxel/ServerChunkManager.hpp"

#include "core/util/util.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <memory>

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

void ServerChunkManager::updatePlayer(entt::entity id, const glm::vec3& world_pos) {
	ChunkCoords coords = toChunkCoords(world_pos);
	coords.y = 0;

	const auto iter = playerChunks.find(id);
	if (iter != playerChunks.end()) {
		auto& [id, state] = *iter;
		if (coords != state.lastPlayerChunkCoord) {
			state.lastPlayerChunkCoord = coords;
			updatePlayerNetChunkState(state);
		}
	}
}

void ServerChunkManager::addPlayer(entt::entity id, const glm::vec3& world_pos) {
	NetChunkState& state = playerChunks[id];
	
	ChunkCoords coords = Voxel::toChunkCoords(world_pos);
	coords.y = 0;
	state.lastPlayerChunkCoord = coords;

	updatePlayerNetChunkState(state);
}

void ServerChunkManager::removePlayer(entt::entity id) {
	playerChunks.erase(id);
}

std::vector<ChunkCoords> ServerChunkManager::popPlayerUnackedChunks(entt::entity id, i32 max_count) {
	std::vector<ChunkCoords> coords;
	coords.reserve(max_count);

	const auto iter = playerChunks.find(id);
	if (iter != playerChunks.end()) {
		auto& [key, state] = *iter;

		i32 num_elements = std::min(max_count, (i32)state.unackedChunks.size());
		
		for (i32 index = 0; index < num_elements; index++) {
			size_t end_index = state.unackedChunks.size() - index - 1;
			const Chunk* chunk = state.unackedChunks[end_index];

			state.chunkAckMap[chunk] = chunk->getUpdateCount();
			coords.push_back(chunk->getChunkCoords());
		}

		state.unackedChunks.resize(state.unackedChunks.size() - num_elements);
	}

	return coords;
}

void ServerChunkManager::updatePlayerNetChunkState(NetChunkState& state) {
	ChunkCoords delta = {RENDER_RADIUS, 0, RENDER_RADIUS};

	state.unackedChunks.clear();

	for (ChunkIndex cx = -RENDER_RADIUS; cx <= RENDER_RADIUS; cx++) {
		for (ChunkIndex cz = -RENDER_RADIUS; cz <= RENDER_RADIUS; cz++) {
			ChunkCoords coords = state.lastPlayerChunkCoord + ChunkCoords{ cx, 0, cz };
			const auto chunk_iter = chunks.find(coords);
			if (chunk_iter != chunks.end()) {
				const auto& [key, chunk] = *chunk_iter;

				const auto& [node, inserted] = state.chunkAckMap.insert(std::make_pair(&chunk, 0));
				if (inserted) {
					state.unackedChunks.push_back(&chunk);
				}
				else if(chunk.getUpdateCount() > node->second) {
					state.unackedChunks.push_back(&chunk);
				}
			}
		}
	}

	const auto manhattan_distance = [](ChunkCoords source, ChunkCoords dest) {
		return glm::compAdd(glm::abs(dest - source));
	};

	ChunkCoords center = state.lastPlayerChunkCoord;
	const auto cmp = [&](const Chunk* left, const Chunk* right) {
		return manhattan_distance(center, left->getChunkCoords()) > manhattan_distance(center, right->getChunkCoords());
	};
	std::sort(state.unackedChunks.begin(), state.unackedChunks.end(), cmp);
}