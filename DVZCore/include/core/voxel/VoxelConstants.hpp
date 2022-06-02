#ifndef DVZ_VOXEL_CONSTANTS_HPP
#define DVZ_VOXEL_CONSTANTS_HPP
#include "core/common.hpp"
#include <glm/glm.hpp>

namespace DVZ::Voxel {
	constexpr i32 CHUNK_SHIFT_X = 4;
	constexpr i32 CHUNK_SHIFT_Y = 7;
	constexpr i32 CHUNK_SHIFT_Z = 4;

	constexpr i32 CHUNK_SHIFT_MAX = glm::max(glm::max(CHUNK_SHIFT_X, CHUNK_SHIFT_Y), CHUNK_SHIFT_Z);

	constexpr i32 CHUNK_X = 1 << CHUNK_SHIFT_X;
	constexpr i32 CHUNK_Y = 1 << CHUNK_SHIFT_Y;
	constexpr i32 CHUNK_Z = 1 << CHUNK_SHIFT_Z;
	constexpr i32 CHUNK_SIZE = CHUNK_X * CHUNK_Y * CHUNK_Z;

	using BlockIndex = u8;
	using ChunkIndex = i32;
	using WorldIndex = i32;

	using BlockCoords = glm::vec<3, BlockIndex>;
	using ChunkCoords = glm::vec<3, ChunkIndex>;
	using WorldCoords = glm::vec<3, WorldIndex>;

	constexpr BlockCoords MAX_BLOCK_COORDS{ CHUNK_X - 1, CHUNK_Y - 1, CHUNK_Z - 1 };
	constexpr BlockCoords MIN_BLOCK_COORDS{ 0, 0, 0 };

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