#include "core/voxel/IChunk.hpp"
#include <assert.h>

using namespace DVZ::Voxel;


IChunk::IChunk(const ChunkCoords& coords) : coords(coords), updateCount(0) {
	data = PoolAllocator<IChunkData>::getInstance()->allocate();
}

const ChunkCoords& IChunk::getChunkCoords() const {
	return coords;
}

BlockType IChunk::getBlock(const BlockCoords& coords) const {
	data->getBlock(coords);
}

bool IChunk::setBlock(const BlockCoords& coords, BlockType type) {
	if (data->setBlock(coords, type)) {
		incrementUpdateCount();
		return true;
	}
	return false;
}

CompressedChunk IChunk::compress() const {
	return CompressedChunk{ getChunkCoords(), getUpdateCount(), data->compress()};
}

bool IChunk::decompress(const CompressedChunk& chunk) {
	coords = chunk.coords;
	updateCount = chunk.updateCount;
	return data->decompress(chunk.data);
}

void IChunk::incrementUpdateCount() {
	updateCount += 1;
}

i32 IChunk::getUpdateCount() const {
	return updateCount;
}