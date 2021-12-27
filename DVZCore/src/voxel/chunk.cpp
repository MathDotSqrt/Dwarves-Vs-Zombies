#include "core/voxel/chunk.hpp"

#include <glm/gtc/noise.hpp>

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

Chunk::Chunk(const ChunkCoords& coords) : Chunk(coords.x, coords.y, coords.z) {
	
}

Chunk::Chunk(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz) : data(std::make_unique<ChunkData>()){
	init(ChunkCoords{ cx, cy, cz });
}

void Chunk::init(const ChunkCoords& coords) {
	this->coords = coords;

	for (BlockIndex z = 0; z < CHUNK_Z; z++) {
		for (BlockIndex x = 0; x < CHUNK_X; x++) {
			WorldCoords worldCoords = toWorldCoords(coords, BlockCoords(x, 0, z));
			glm::vec2 sample{ worldCoords.x / 100.0f, worldCoords.z / 100.0f};
			double height = (glm::simplex(sample) + 1) * .5;
			
			height = glm::pow(height, 2);

			int worldHeight = glm::clamp((int)(height * CHUNK_Y), 3, CHUNK_Y);

			for (BlockIndex y = 0; y < CHUNK_Y; y++) {
				if (y < 3) {
					data->setBlock(x, y, z, BlockType::SAND);
				}
				else if (y < (worldHeight - 1)) {
					data->setBlock(x, y, z, BlockType::DIRT);
				}
				else if (y == (worldHeight - 1)) {
					data->setBlock(x, y, z, BlockType::GRASS);
				}
				else {
					data->setBlock(x, y, z, BlockType::AIR);
				}
			}
		}
	}

	//for (BlockIndex z = 0; z < CHUNK_Z; z++) {
	//	for (BlockIndex x = 0; x < CHUNK_X; x++) {
	//		for (BlockIndex y = 0; y < CHUNK_Y; y++) {
	//			if (y < 10)
	//				data->setBlock(x, y, z, ((coords.x + coords.z) % 2 == 0) ? BlockType::DIRT : BlockType::SAND);
	//			else
	//				data->setBlock(x, y, z, BlockType::AIR);
	//		}
	//	}
	//}
	updateCount = 1;

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
		incrementUpdateCount();
	}
}

void Chunk::setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block) {
	bool isupdate = data->setBlock(bx, by, bz, block);
	if (isupdate) {
		incrementUpdateCount();
	}
}

const ChunkCoords& Chunk::getChunkCoords() const {
	return coords;
}

void Chunk::incrementUpdateCount() {
	updateCount += 1;
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
	return WorldCoords{chunkCoords.x * CHUNK_X + blockCoords.x, chunkCoords.y * CHUNK_Y + blockCoords.y, chunkCoords.z * CHUNK_Z + blockCoords.z };
}

DVZ::Collision::AABB DVZ::Voxel::getChunkAABB(const ChunkCoords& coords) {
	glm::vec3 size{ CHUNK_X, CHUNK_Y, CHUNK_Z };
	
	glm::vec3 min = glm::vec3(coords) * size;
	glm::vec3 max = min + size;
	return { min, max };
}