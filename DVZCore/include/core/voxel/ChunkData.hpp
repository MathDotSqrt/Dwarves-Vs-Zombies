#include "core/voxel/VoxelConstants.hpp"
#include "core/voxel/block.hpp"

#include <vector>
#include <array>

namespace DVZ::Voxel {
	class IChunkData {
	public:
		BlockType getBlock(const BlockCoords& coords) const;
		bool setBlock(const BlockCoords& coords, BlockType type);

		std::vector<u8> compress() const;
		bool decompress(const std::vector<u8>& data);

	private:
		i32 toIndex(const BlockCoords& coords) const;

		std::array<BlockType, CHUNK_SIZE> data;
	};
}