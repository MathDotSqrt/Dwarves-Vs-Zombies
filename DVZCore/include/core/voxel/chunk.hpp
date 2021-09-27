#pragma once
#ifndef DVZ_CHUNK_HPP
#define DVZ_CHUNK_HPP

#include "core/voxel/block.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>


namespace DVZ::Voxel {
	
	constexpr int CHUNK_X = 16;
	constexpr int CHUNK_Y = 256;
	constexpr int CHUNK_Z = 16;
	constexpr int CHUNK_SIZE = CHUNK_X * CHUNK_Y * CHUNK_Z;

	using BlockIndex = u8;
	using ChunkIndex = i32;

	using BlockCoords = glm::vec<3, BlockIndex>;
	using ChunkCoords = glm::vec<3, ChunkIndex>;

	class ChunkData {
	public:
		BlockType getBlock(const BlockCoords& coords) const;
		BlockType getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const;

		void setBlock(const BlockCoords& coords, BlockType block);
		void setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block);
	private:
		int toIndex(const BlockCoords& coords) const;
		int toIndex(BlockIndex bx, BlockIndex by, BlockIndex bz) const;

		std::array<BlockType, CHUNK_SIZE> block_data;

	};
	
	class Chunk {
	public:
		Chunk(ChunkIndex x, ChunkIndex y, ChunkIndex z);
	private:
		ChunkIndex cx;
		ChunkIndex cy;
		ChunkIndex cz;

		std::unique_ptr<ChunkData> data;
	};
}

#endif