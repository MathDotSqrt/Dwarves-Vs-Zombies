#include "core/voxel/chunk.hpp"

#include <glm/gtc/noise.hpp>

#include <assert.h>

using namespace DVZ::Voxel;

ChunkData::ChunkData() : block_data(std::make_unique<std::array<BlockType, CHUNK_SIZE>>()) {

}

BlockType ChunkData::getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const {
	return (*block_data)[toIndex(bx, by, bz)];
}

BlockType ChunkData::getBlock(const BlockCoords& coords) const {
	return (*block_data)[toIndex(coords)];
}

bool ChunkData::setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block) {
	auto index = toIndex(bx, by, bz);
	BlockType prev = (*block_data)[index];
	(*block_data)[index] = block;

	return block != prev;
}

bool ChunkData::setBlock(const BlockCoords& coords, BlockType block) {
	auto index = toIndex(coords);
	BlockType prev = (*block_data)[index];
	(*block_data)[index] = block;

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

std::vector<u8> ChunkData::compressData() const {
	std::vector<u8> compressed_data;

	BlockType last_block = block_data->front();
	u8 length = 0;
	for (const BlockType& data : *block_data) {
		if (data == last_block && length < std::numeric_limits<u8>::max()) {
			++length;
		}
		else{
			u8 lo = static_cast<u8>(length);
			//u8 hi = static_cast<u8>(length >> 8);

			compressed_data.push_back(static_cast<u8>(lo));
			//compressed_data.push_back(static_cast<u8>(hi));
			compressed_data.push_back(static_cast<u8>(last_block));

			last_block = data;
			length = 1;
		}
	}

	return compressed_data;
}

bool ChunkData::decompressData(const std::vector<u8>& compressed_data) {
	constexpr u8 NUM_BLOCK_TYPES = static_cast<u8>(BlockType::NUM_BLOCKS);

	if ((compressed_data.size() & 0b1) == 0b1) {
		return false;
	}

	size_t block_data_index = 0;

	for (size_t index = 0; index < compressed_data.size(); index += 2) {
		u8 count = compressed_data[index];
		u8 value = compressed_data[index + 1];

		if (value >= NUM_BLOCK_TYPES) {
			return false;
		}
		if ((block_data_index + count) >= block_data->size()) {
			return false;
		}

		BlockType block = static_cast<BlockType>(value);
		std::fill_n(block_data->begin() + block_data_index, count, block);
		block_data_index += count;
	}

	return true;
}

Chunk::Chunk(const ChunkCoords& coords) : Chunk(coords.x, coords.y, coords.z) {
	
}

Chunk::Chunk(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz) {
	init(ChunkCoords{ cx, cy, cz });
}

void Chunk::init(const ChunkCoords& coords) {
	this->coords = coords;
	this->updateCount = 0;
}

void Chunk::generate(const ChunkCoords& coords) {
	init(coords);
	for (BlockIndex z = 0; z < CHUNK_Z; z++) {
		for (BlockIndex x = 0; x < CHUNK_X; x++) {
			WorldCoords worldCoords = toWorldCoords(coords, BlockCoords(x, 0, z));
			glm::vec2 sample{ worldCoords.x / 100.0f, worldCoords.z / 100.0f };
			double height = (glm::simplex(sample) + 1) * .5;

			height = glm::pow(height, 2);

			int worldHeight = glm::clamp((int)(height * CHUNK_Y), 3, CHUNK_Y);

			for (BlockIndex y = 0; y < CHUNK_Y; y++) {
				if (y < 3) {
					data.setBlock(x, y, z, BlockType::SAND);
				}
				else if (y < (worldHeight - 1)) {
					data.setBlock(x, y, z, BlockType::DIRT);
				}
				else if (y == (worldHeight - 1)) {
					data.setBlock(x, y, z, BlockType::GRASS);
				}
				else {
					data.setBlock(x, y, z, BlockType::AIR);
				}
			}


		}
	}


	const auto octave_noise = [](const WorldCoords& world_pos) {
		const auto sample_0 = glm::vec2{ world_pos.x, world_pos.z } / 100.0f;

		return glm::simplex(sample_0);

	};

	for (BlockIndex z = 0; z < CHUNK_Z; z++) {
		for (BlockIndex x = 0; x < CHUNK_X; x++) {



		}
	}

	updateCount += 1;
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
}

BlockType Chunk::getBlock(const BlockCoords& coords) const {
	return data.getBlock(coords);
}

BlockType Chunk::getBlock(BlockIndex bx, BlockIndex by, BlockIndex bz) const {
	return data.getBlock(bx, by, bz);
}

void Chunk::setBlock(const BlockCoords& coords, BlockType block) {
	bool isupdate = data.setBlock(coords, block);
	if (isupdate) {
		incrementUpdateCount();
	}
}

void Chunk::setBlock(BlockIndex bx, BlockIndex by, BlockIndex bz, BlockType block) {
	bool isupdate = data.setBlock(bx, by, bz, block);
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

CompressedChunk Chunk::compressChunk() const {
	CompressedChunk compressed{coords, getUpdateCount(), data.compressData()};
	return compressed;
}

bool Chunk::decompressChunk(const CompressedChunk& compressed_data) {
	coords = compressed_data.coords;
	updateCount = compressed_data.updateCount;
	return data.decompressData(compressed_data.data);
}

//BlockIndex DVZ::Voxel::toBlockXIndex(WorldIndex index) {
//	constexpr int BLOCK_MASK = (1 << CHUNK_SHIFT_X) - 1;
//	return BLOCK_MASK & index;
//}
//
//BlockIndex DVZ::Voxel::toBlockYIndex(WorldIndex index) {
//	constexpr int BLOCK_MASK = (1 << CHUNK_SHIFT_Y) - 1;
//	return BLOCK_MASK & index;
//}
//
//BlockIndex DVZ::Voxel::toBlockZIndex(WorldIndex index) {
//	constexpr int BLOCK_MASK = (1 << CHUNK_SHIFT_Z) - 1;
//	return BLOCK_MASK & index;
//}
//
//
//ChunkIndex DVZ::Voxel::toChunkXIndex(WorldIndex index) {
//	return index >> CHUNK_SHIFT_X;
//}
//
//ChunkIndex DVZ::Voxel::toChunkYIndex(WorldIndex index) {
//	return index >> CHUNK_SHIFT_Y;
//}
//
//ChunkIndex DVZ::Voxel::toChunkZIndex(WorldIndex index) {
//	return index >> CHUNK_SHIFT_Z;
//}
//
//BlockCoords DVZ::Voxel::toBlockCoords(const WorldCoords& coords) {
//	return BlockCoords{ toBlockXIndex(coords.x), toBlockYIndex(coords.y), toBlockZIndex(coords.z) };
//}
//
//ChunkCoords DVZ::Voxel::toChunkCoords(const WorldCoords& coords) {
//	return ChunkCoords{ toChunkXIndex(coords.x), toChunkYIndex(coords.y), toChunkZIndex(coords.z) };
//}
//
//ChunkCoords DVZ::Voxel::toChunkCoords(const glm::vec3& coords) {
//	return toChunkCoords(toWorldCoords(coords));
//}
//
//WorldCoords DVZ::Voxel::toWorldCoords(const glm::vec3& coords) {
//	return WorldCoords{ glm::floor(coords) };
//}
//
//WorldCoords DVZ::Voxel::toWorldCoords(const ChunkCoords& chunkCoords, const BlockCoords& blockCoords) {
//	return WorldCoords{chunkCoords.x * CHUNK_X + blockCoords.x, chunkCoords.y * CHUNK_Y + blockCoords.y, chunkCoords.z * CHUNK_Z + blockCoords.z };
//}

DVZ::Collision::AABB DVZ::Voxel::getChunkAABB(const ChunkCoords& coords) {
	glm::vec3 size{ CHUNK_X, CHUNK_Y, CHUNK_Z };
	
	glm::vec3 min = glm::vec3(coords) * size;
	glm::vec3 max = min + size;
	return { min, max };
}
