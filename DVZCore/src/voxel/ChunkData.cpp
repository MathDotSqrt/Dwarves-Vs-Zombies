#include "core/voxel/ChunkData.hpp"

using namespace DVZ::Voxel;


BlockType IChunkData::getBlock(const BlockCoords& coords) const {
	return data[toIndex(coords)];
}

bool IChunkData::setBlock(const BlockCoords& coords, BlockType type) {
	BlockType& current_block = data[toIndex(coords)];
	if (current_block == type) {
		return false;
	}
	current_block = type;
	return true;
}

i32 IChunkData::toIndex(const BlockCoords& coords) const {
	assert(coords.x >= 0 && coords.x < CHUNK_X);
	assert(coords.y >= 0 && coords.y < CHUNK_Y);
	assert(coords.z >= 0 && coords.z < CHUNK_Z);

	int index = (CHUNK_X * CHUNK_Z * coords.y) + (coords.z * CHUNK_X) + coords.x;
	return index;
}