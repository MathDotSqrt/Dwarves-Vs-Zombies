#include "core/voxel/chunk.hpp"

#include <assert.h>

using namespace DVZ::Voxel;

BlockType ChunkData::getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const {
	return block_data[toIndex(bx, by, bz)];
}

BlockType ChunkData::getBlock(const BlockCoords& coords) const {
	return block_data[toIndex(coords)];
}

bool ChunkData::setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block) {
	auto index = toIndex(bx, by, bz);
	BlockType prev = block_data[index];
	block_data[index] = block;

	return block != prev;
}

bool ChunkData::setBlock(const BlockCoords& coords, BlockType block) {
	auto index = toIndex(coords);
	BlockType prev = block_data[index];
	block_data[index] = block;

	return block != prev;
}

int ChunkData::toIndex(const BlockCoords& coords) const {
	return toIndex(coords.x, coords.y, coords.z);
}

int ChunkData::toIndex(BlockIndex bx, BlockIndex by, BlockIndex bz) const {
	assert(bx >= 0 && bx < CHUNK_X);
	assert(by >= 0 && by < CHUNK_Y);
	assert(bz >= 0 && bz < CHUNK_Z);

	int index = (CHUNK_X * CHUNK_Z * by) + (bz * CHUNK_X) + bx;
	return index;
}

Chunk::Chunk(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz) : data(std::make_unique<ChunkData>()){
	coords = ChunkCoords{cx, cy, cz};

	BlockIndex y = 0;
	while(true) {
		for (BlockIndex z = 0; z < CHUNK_Z; z++) {
			for (BlockIndex x = 0; x < CHUNK_X; x++) {
				if (y < 10)
					data->setBlock(x, y, z, BlockType::GRASS);
				else
					data->setBlock(x, y, z, BlockType::AIR);
			}
		}

		//scuff
		if (y == (CHUNK_Y - 1)) {
			break;
		}

		y += 1;
	}

	updateCount += 1;
}

BlockType Chunk::getBlock(const BlockCoords& coords) const {
	return data->getBlock(coords);
}

BlockType Chunk::getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const {
	return data->getBlock(bx, by, bz);
}

void Chunk::setBlock(const BlockCoords& coords, BlockType block) {
	bool isupdate = data->setBlock(coords, block);
	if (isupdate) {
		updateCount += 1;
	}
}

void Chunk::setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block) {
	bool isupdate = data->setBlock(bx, by, bz, block);
	if (isupdate) {
		updateCount += 1;
	}
}

const ChunkCoords& Chunk::getChunkCoords() const {
	return coords;
}

int Chunk::getUpdateCount() const {
	return updateCount;
}

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

ChunkCoords DVZ::Voxel::toChunkCoords(const WorldCoords& coords) {
	return ChunkCoords{ toChunkXIndex(coords.x), toChunkYIndex(coords.y), toChunkZIndex(coords.z) };
}

ChunkCoords DVZ::Voxel::toChunkCoords(const glm::vec3& coords) {
	return toChunkCoords(toWorldCoords(coords));
}

WorldCoords DVZ::Voxel::toWorldCoords(const glm::vec3& coords) {
	return WorldCoords{ glm::floor(coords / BLOCK_WIDTH) };
}