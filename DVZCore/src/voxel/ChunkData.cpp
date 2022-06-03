#include "core/voxel/ChunkData.hpp"

using namespace DVZ::Voxel;


BlockType IChunkData::getBlock(const BlockCoords& coords) const {
	return block_data[toIndex(coords)];
}

bool IChunkData::setBlock(const BlockCoords& coords, BlockType type) {
	BlockType& current_block = block_data[toIndex(coords)];
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

std::vector<u8> IChunkData::compress() const {
	std::vector<u8> compressed_data;

	BlockType last_block = block_data.front();
	u8 length = 0;
	for (const BlockType& data : block_data) {
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

bool IChunkData::decompress(const std::vector<u8>& compressed_data) {
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
		if ((block_data_index + count) >= block_data.size()) {
			return false;
		}
	
		BlockType block = static_cast<BlockType>(value);
		std::fill_n(block_data.begin() + block_data_index, count, block);
		block_data_index += count;
	}
	
	return true;
}