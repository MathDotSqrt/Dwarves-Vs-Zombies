#pragma once
#ifndef DVZ_CHUNK_RENDER_DATA_MANAGER_HPP
#define DVZ_CHUNK_RENDER_DATA_MANAGER_HPP

#include "client/voxel/ChunkRenderData.hpp"

#include <vector>
#include <mutex>
#include <array>
#include <memory>
#include <unordered_map>

namespace DVZ::Voxel {

	class ClientChunkManager;

	class ChunkMesher {
	public:
		ChunkMesher();

		void loadChunkData();
	private:
		constexpr static size_t PADDED_CHUNK_X = CHUNK_X + 2;
		constexpr static size_t PADDED_CHUNK_Y = CHUNK_Y + 2;
		constexpr static size_t PADDED_CHUNK_Z = CHUNK_Z + 2;
		constexpr static size_t PADDED_VOLUME = PADDED_CHUNK_X * PADDED_CHUNK_Y * PADDED_CHUNK_Z;

		using BlockStorage = std::array<BlockType, PADDED_VOLUME>;

		std::unique_ptr<BlockStorage> blocks;
	};

	class ChunkRenderDataManager {
	public:

		void bufferDirtyChunks(const ClientChunkManager& chunkManager);

	private:
		std::mutex queue_mutex;
		std::vector<ChunkMesher> queuedChunks;
		std::vector<ChunkRenderData> renderableChunks;

		std::unordered_map<ChunkCoords, int> chunkMeshUpdateCountMap;
	};
}

#endif