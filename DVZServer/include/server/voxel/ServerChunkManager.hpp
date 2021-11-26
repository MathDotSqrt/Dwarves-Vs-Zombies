#ifndef DVZ_SERVER_CHUNK_MANAGER_HPP
#define DVZ_SERVER_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"
#include <unordered_map>

namespace DVZ::Voxel {
	class ServerChunkManager {
	public:
		ServerChunkManager();

		const Chunk* getChunk(const ChunkCoords& coords) const;
		BlockType getBlock(const WorldCoords& coords) const;
	private:
		constexpr static ChunkIndex RENDER_RADIUS = 5;

		std::unordered_map<ChunkCoords, Chunk> chunks;
	};
}

#endif