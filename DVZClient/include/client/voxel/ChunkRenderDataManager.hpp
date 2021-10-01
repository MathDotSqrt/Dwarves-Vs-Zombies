#pragma once
#ifndef DVZ_CHUNK_RENDER_DATA_MANAGER_HPP
#define DVZ_CHUNK_RENDER_DATA_MANAGER_HPP

#include "client/voxel/ChunkMesher.hpp"
#include "client/voxel/ChunkRenderData.hpp"

#include <vector>
#include <mutex>
#include <array>
#include <memory>
#include <queue>
#include <unordered_map>

namespace DVZ::Voxel {

	class ClientChunkManager;


	class ChunkRenderDataManager {
	public:
		void bufferDirtyChunks(const ClientChunkManager& chunkManager);
		void meshChunks();
		const std::vector<ChunkRenderData>& getRenderableChunks() const;
	private:
		constexpr static size_t MAX_CHUNK_MESH_QUEUE = 16;

		int chunkDistance(const ChunkCoords& coords) const;

		ChunkCoords playerCoords{0};

		std::mutex queue_mutex;
		std::vector<ChunkMesher> queuedChunks;
		std::vector<ChunkMesher> mesherPool;

		std::vector<ChunkRenderData> renderableChunks;
		std::unordered_map<ChunkCoords, int> chunkMeshUpdateCountMap;
	};
}

#endif