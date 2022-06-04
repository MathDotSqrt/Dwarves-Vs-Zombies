#pragma once
#ifndef DVZ_CHUNK_RENDER_DATA_MANAGER_HPP
#define DVZ_CHUNK_RENDER_DATA_MANAGER_HPP

#include "client/voxel/ChunkMesher.hpp"
#include "client/voxel/ChunkRenderData.hpp"
#include "core/util/Frustum.hpp"
#include "core/util/PoolAllocator.hpp"

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
		void bufferDirtyChunks(const Frustum& frustum, const ClientChunkManager& chunkManager);
		void update();
		void clearRenderData();

		const std::unordered_map<ChunkCoords, ChunkRenderData>& getRenderableChunks() const;
	private:
#ifdef DVZ_CLIENT_DEBUG
		constexpr static size_t MAX_THREADS = 1;
#else
		constexpr static size_t MAX_THREADS = 8;
#endif
		constexpr static size_t MIN_CHUNK_MESH = 3;
		constexpr static size_t MAX_CHUNK_MESH_QUEUE = MAX_THREADS * 2 + MIN_CHUNK_MESH;
		
		std::vector<ChunkCoords> getSortedCoords(const ChunkCoords& playerCoords, const Frustum& frustum) const;
		void cullFarChunks();
		void meshChunks();
		void meshChunksOnThread();
		void launchMesherThreads();
		void bufferMeshedChunks();
		AllocatorHandle<ChunkMesher> meshChunkAsync(AllocatorHandle<ChunkMesher>&&);
		int chunkDistance(const ChunkCoords& coords) const;

		ChunkCoords playerCoords{0};

		std::mutex queue_mutex;
		std::vector<AllocatorHandle<ChunkMesher>> queuedChunks;
		//std::vector<ChunkMesher> mesherPool;

		std::unordered_map<ChunkCoords, ChunkRenderData> renderableChunks;

		std::unordered_map<ChunkCoords, i32> chunkMeshUpdateCountMap;
		std::vector<std::future<AllocatorHandle<ChunkMesher>>> futureChunkGeometries;
	};
}

#endif