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
#include <future>
#include <unordered_map>

namespace DVZ::Voxel {

	class ClientChunkManager;


	class ChunkRenderDataManager {
	public:
		void bufferDirtyChunks(const ClientChunkManager& chunkManager);
		void update();
		void clearRenderData();

		const std::vector<ChunkRenderData>& getRenderableChunks() const;
	private:
#ifdef DVZ_CLIENT_DEBUG
		constexpr static size_t MAX_THREADS = 1;
#else
		constexpr static size_t MAX_THREADS = 8;
#endif
		constexpr static size_t MAX_CHUNK_MESH_QUEUE = MAX_THREADS * 2;


		void cullFarChunks();
		void meshChunks();
		void launchMesherThreads();
		void bufferMeshedChunks();
		ChunkMesher meshChunkAsync(ChunkMesher &&);
		int chunkDistance(const ChunkCoords& coords) const;

		ChunkCoords playerCoords{0};

		std::mutex queue_mutex;
		std::vector<ChunkMesher> queuedChunks;
		std::vector<ChunkMesher> mesherPool;

		std::vector<ChunkRenderData> renderableChunks;
		std::unordered_map<ChunkCoords, int> chunkMeshUpdateCountMap;
		std::vector<std::future<ChunkMesher>> futureChunkGeometries;
	};
}

#endif