#include "core/voxel/VoxelConstants.hpp"

using namespace DVZ::Voxel;

BlockIndex DVZ::Voxel::toBlockXIndex(WorldIndex index) {
	constexpr int BLOCK_MASK = (1 << CHUNK_SHIFT_X) - 1;
	return BLOCK_MASK & index;
}

BlockIndex DVZ::Voxel::toBlockYIndex(WorldIndex index) {
	constexpr int BLOCK_MASK = (1 << CHUNK_SHIFT_Y) - 1;
	return BLOCK_MASK & index;
}

BlockIndex DVZ::Voxel::toBlockZIndex(WorldIndex index) {
	constexpr int BLOCK_MASK = (1 << CHUNK_SHIFT_Z) - 1;
	return BLOCK_MASK & index;
}


ChunkIndex DVZ::Voxel::toChunkXIndex(WorldIndex index) {
	return index >> CHUNK_SHIFT_X;
}

ChunkIndex DVZ::Voxel::toChunkYIndex(WorldIndex index) {
	return index >> CHUNK_SHIFT_Y;
}

ChunkIndex DVZ::Voxel::toChunkZIndex(WorldIndex index) {
	return index >> CHUNK_SHIFT_Z;
}

BlockCoords DVZ::Voxel::toBlockCoords(const WorldCoords& coords) {
	return BlockCoords{ toBlockXIndex(coords.x), toBlockYIndex(coords.y), toBlockZIndex(coords.z) };
}

ChunkCoords DVZ::Voxel::toChunkCoords(const WorldCoords& coords) {
	return ChunkCoords{ toChunkXIndex(coords.x), toChunkYIndex(coords.y), toChunkZIndex(coords.z) };
}

ChunkCoords DVZ::Voxel::toChunkCoords(const glm::vec3& coords) {
	return toChunkCoords(toWorldCoords(coords));
}

WorldCoords DVZ::Voxel::toWorldCoords(const glm::vec3& coords) {
	return WorldCoords{ glm::floor(coords) };
}

WorldCoords DVZ::Voxel::toWorldCoords(const ChunkCoords& chunkCoords, const BlockCoords& blockCoords) {
	return WorldCoords{ chunkCoords.x * CHUNK_X + blockCoords.x, chunkCoords.y * CHUNK_Y + blockCoords.y, chunkCoords.z * CHUNK_Z + blockCoords.z };
}