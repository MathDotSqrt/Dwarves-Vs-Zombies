#ifndef DVZ_SERVER_CHUNK_MANAGER_HPP
#define DVZ_SERVER_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"
#include <unordered_map>
#include <entt/entt.hpp>

namespace DVZ::Voxel {

	struct NetChunkState {
		ChunkCoords lastPlayerChunkCoord = ChunkCoords{0, 0, 0};
		std::unordered_map<const Chunk*, int> chunkAckMap;

		using node_ptr = std::unordered_map<const Chunk*, int>::pointer;
		std::vector<const Chunk*> unackedChunks;
	};

	class ServerChunkManager {
	public:
		ServerChunkManager();

		const Chunk* getChunk(const ChunkCoords& coords) const;
		BlockType getBlock(const WorldCoords& coords) const;

		void updatePlayer(entt::entity id, const glm::vec3& world_position);
		void addPlayer(entt::entity id, const glm::vec3& world_position);
		void removePlayer(entt::entity id);

		std::vector<ChunkCoords> popPlayerUnackedChunks(entt::entity id, i32 max_count=8);
	private:
		constexpr static ChunkIndex WORLD_RADIUS = 30;
		constexpr static ChunkIndex RENDER_RADIUS = 5;

		void updatePlayerNetChunkState(NetChunkState& state);

		std::unordered_map<ChunkCoords, Chunk> chunks;

		std::unordered_map<ChunkCoords, CompressedChunk> compressedChunksCache;

		std::unordered_map<entt::entity, NetChunkState> playerChunks;
	};
}

#endif