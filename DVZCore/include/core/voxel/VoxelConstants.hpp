
#include "core/common.hpp"
#include <glm/glm.hpp>

namespace DVZ::Voxel {
	constexpr i32 CHUNK_SHIFT_X = 4;
	constexpr i32 CHUNK_SHIFT_Y = 7;
	constexpr i32 CHUNK_SHIFT_Z = 4;

	constexpr i32 CHUNK_SHIFT_MAX = std::max(std::max(CHUNK_SHIFT_X, CHUNK_SHIFT_Y), CHUNK_SHIFT_Z);

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

}