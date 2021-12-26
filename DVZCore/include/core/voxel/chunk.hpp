#pragma once
#ifndef DVZ_CHUNK_HPP
#define DVZ_CHUNK_HPP

#include "core/voxel/block.hpp"
#include "core/collision/AABB.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>


namespace DVZ::Voxel {
	
	constexpr int CHUNK_SHIFT_X = 4;
	constexpr int CHUNK_SHIFT_Y = 7;
	constexpr int CHUNK_SHIFT_Z = 4;

	constexpr int CHUNK_SHIFT_MAX = std::max(std::max(CHUNK_SHIFT_X, CHUNK_SHIFT_Y), CHUNK_SHIFT_Z);

	constexpr int CHUNK_X = 1 << CHUNK_SHIFT_X;
	constexpr int CHUNK_Y = 1 << CHUNK_SHIFT_Y;
	constexpr int CHUNK_Z = 1 << CHUNK_SHIFT_Z;
	constexpr int CHUNK_SIZE = CHUNK_X * CHUNK_Y * CHUNK_Z;

	using BlockIndex = u8;
	using ChunkIndex = i32;
	using WorldIndex = i32;

	using BlockCoords = glm::vec<3, BlockIndex>;
	using ChunkCoords = glm::vec<3, ChunkIndex>;
	using WorldCoords = glm::vec<3, WorldIndex>;

	constexpr BlockCoords MAX_BLOCK_COORDS{CHUNK_X - 1, CHUNK_Y  - 1, CHUNK_Z - 1};
	constexpr BlockCoords MIN_BLOCK_COORDS{ 0, 0, 0 };

	class ChunkData {
	public:
		BlockType getBlock(const BlockCoords& coords) const;
		BlockType getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const;

		bool setBlock(const BlockCoords& coords, BlockType block);
		bool setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block);
	private:
		int toIndex(const BlockCoords& coords) const;
		int toIndex(BlockIndex bx, BlockIndex by, BlockIndex bz) const;

		std::array<BlockType, CHUNK_SIZE> block_data;

	};
	
	class Chunk {
	public:
		Chunk(const ChunkCoords& coords);
		Chunk(ChunkIndex x=0, ChunkIndex y=0, ChunkIndex z=0);

		void init(const ChunkCoords& coords);

		BlockType getBlock(const BlockCoords& coords) const;
		BlockType getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const;

		void setBlock(const BlockCoords& coords, BlockType block);
		void setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block);

		const ChunkCoords& getChunkCoords() const;
		int getUpdateCount() const;
	private:
		ChunkCoords coords;
		int updateCount = 0;

		std::unique_ptr<ChunkData> data;
	};

	BlockIndex toBlockXIndex(WorldIndex index);
	BlockIndex toBlockYIndex(WorldIndex index);
	BlockIndex toBlockZIndex(WorldIndex index);
	ChunkIndex toChunkXIndex(WorldIndex index);
	ChunkIndex toChunkYIndex(WorldIndex index);
	ChunkIndex toChunkZIndex(WorldIndex index);

	BlockCoords toBlockCoords(const WorldCoords& coords);
	ChunkCoords toChunkCoords(const WorldCoords& coords);
	ChunkCoords toChunkCoords(const glm::vec3& coords);
	WorldCoords toWorldCoords(const glm::vec3& coords);
	WorldCoords toWorldCoords(const ChunkCoords& chunkCoords, const BlockCoords& blockCoords);

	Collision::AABB getChunkAABB(const ChunkCoords& chunkCoords);
}

namespace std {
	template<>
	struct hash<DVZ::Voxel::ChunkCoords> {

		size_t expand(size_t x) const {
			x &= 0x3FF;
			x = (x | (x << 16)) & 4278190335;
			x = (x | (x << 8)) & 251719695;
			x = (x | (x << 4)) & 3272356035;
			x = (x | (x << 2)) & 1227133513;
			return x;
		}

		std::size_t operator()(const DVZ::Voxel::ChunkCoords& coords) const {
			return expand(coords.x) + (expand(coords.y) << 1) + (expand(coords.z) << 2);
		}
	};
}

#endif