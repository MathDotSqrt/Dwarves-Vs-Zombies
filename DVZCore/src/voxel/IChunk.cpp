#include "core/voxel/IChunk.hpp"
#include <assert.h>

#include <glm/gtc/noise.hpp>

using namespace DVZ::Voxel;


IChunk::IChunk(const ChunkCoords& coords) : coords(coords), updateCount(0) {
	data = PoolAllocator<IChunkData>::getInstance(30 * 30)->allocate();
}

const ChunkCoords& IChunk::getChunkCoords() const {
	return coords;
}

BlockType IChunk::getBlock(const BlockCoords& coords) const {
	return data->getBlock(coords);
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

void IChunk::generate() {

	for (BlockIndex z = 0; z < CHUNK_Z; z++) {
		for (BlockIndex x = 0; x < CHUNK_X; x++) {
			WorldCoords worldCoords = toWorldCoords(coords, BlockCoords(x, 0, z));
			glm::vec2 sample{ worldCoords.x / 100.0f, worldCoords.z / 100.0f };
			double height = (glm::simplex(sample) + 1) * .5;

			height = glm::pow(height, 2);

			int worldHeight = glm::clamp((int)(height * CHUNK_Y), 3, CHUNK_Y);

			for (BlockIndex y = 0; y < CHUNK_Y; y++) {
				BlockCoords coords{x, y, z};
				if (y < 3) {
					data->setBlock(coords, BlockType::SAND);
				}
				else if (y < (worldHeight - 1)) {
					data->setBlock(coords, BlockType::DIRT);
				}
				else if (y == (worldHeight - 1)) {
					data->setBlock(coords, BlockType::GRASS);
				}
				else {
					data->setBlock(coords, BlockType::AIR);
				}
			}


		}
	}

	//for (BlockIndex bz = 0; bz < CHUNK_Z; bz++) {
	//	for (BlockIndex bx = 0; bx < CHUNK_X; bx++) {
	//		for (BlockIndex by = 0; by < CHUNK_Y; by++) {
	//			if(by > 90){
	//				data->setBlock(BlockCoords{bx, by, bz}, BlockType::AIR);
	//			}
	//			else if(by == 90) {
	//				data->setBlock(BlockCoords{ bx, by, bz }, BlockType::GRASS);
	//			}
	//			else {
	//				data->setBlock(BlockCoords{ bx, by, bz }, BlockType::DIRT);
	//			}
	//		}
	//	}
	//}
	incrementUpdateCount();
}

void IChunk::incrementUpdateCount() {
	updateCount += 1;
}

i32 IChunk::getUpdateCount() const {
	return updateCount;
}