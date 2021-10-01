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

	class ChunkNeighbors;
	class ClientChunkManager;


	class ChunkMesher {
	public:
		ChunkMesher();

		//TODO: implement chunk neighbors
		void loadChunkData(const ChunkNeighbors& neighbors);
		const ChunkVertexVector& meshChunk();
		void setCoords(const ChunkCoords& newCoords);
		const ChunkCoords& getCoords() const;
	private:
		constexpr static size_t PADDED_CHUNK_X = CHUNK_X + 2;
		constexpr static size_t PADDED_CHUNK_Y = CHUNK_Y + 2;
		constexpr static size_t PADDED_CHUNK_Z = CHUNK_Z + 2;
		constexpr static size_t PADDED_VOLUME = PADDED_CHUNK_X * PADDED_CHUNK_Y * PADDED_CHUNK_Z;

		using BlockStorage = std::array<BlockType, PADDED_VOLUME>;

		struct BlockFaceCullTags {
			bool nx : 1;
			bool px : 1;
			bool ny : 1;
			bool py : 1;
			bool nz : 1;
			bool pz : 1;
		};

		ChunkCoords coords;

		void appendCubeGeometry(const BlockCoords& coords, BlockFaceCullTags tags);
		BlockType& getPaddedBlock(int bx, int by, int bz);

		std::unique_ptr<BlockStorage> blocks;
		ChunkVertexVector geometry;
	};

	class ChunkRenderDataManager {
	public:
		
		void bufferDirtyChunks(const ClientChunkManager& chunkManager);
		void meshChunks();
		const std::vector<ChunkRenderData>& getRenderableChunks() const;
	private:
		size_t MAX_CHUNK_MESH_QUEUE = 16;

		ChunkCoords playerCoords{0};

		std::mutex queue_mutex;
		std::vector<ChunkMesher> queuedChunks;
		std::vector<ChunkMesher> mesherPool;

		std::vector<ChunkRenderData> renderableChunks;
		std::unordered_map<ChunkCoords, int> chunkMeshUpdateCountMap;
	};
}

#endif