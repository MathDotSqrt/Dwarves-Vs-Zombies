#pragma once 
#ifndef DVZ_CHUNK_MESHER_HPP
#define DVZ_CHUNK_MESHER_HPP

#include "core/voxel/chunk.hpp"
#include "client/voxel/ChunkRenderData.hpp"

namespace DVZ::Voxel {
	class ChunkNeighbors;

	class ChunkMesher {
	public:
		ChunkMesher();

		//TODO: implement chunk neighbors
		void loadChunkData(const ChunkNeighbors& neighbors);
		const ChunkVertexVector& meshChunk();
		void setCoords(const ChunkCoords& newCoords);
		const ChunkCoords& getCoords() const;

		const ChunkVertexVector& getGeometry() const;

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
}

#endif