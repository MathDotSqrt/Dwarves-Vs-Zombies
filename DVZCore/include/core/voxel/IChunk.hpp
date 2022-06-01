#include "core/voxel/block.hpp"
#include "core/voxel/VoxelConstants.hpp"
#include "core/voxel/ChunkData.hpp"
#include "core/util/PoolAllocator.hpp"

#include <vector>
#include <array>
#include <memory>
#include <functional>

namespace DVZ::Voxel {

	struct CompressedChunk {
		ChunkCoords coords ;
		i32 updateCount;
		std::vector<u8> data;
	};

	class IChunk {
	public:
		IChunk(const ChunkCoords& coords);
		
		const ChunkCoords& getChunkCoords() const;

		virtual BlockType getBlock(const BlockCoords& coords) const;
		virtual bool setBlock(const BlockCoords& coords, BlockType block);

		virtual CompressedChunk compress() const;
		virtual bool decompress(const CompressedChunk& chunk);

		void incrementUpdateCount();
		i32 getUpdateCount() const;

	protected:
		ChunkCoords coords = ChunkCoords{0};
		i32 updateCount = 0;

		AllocatorHandle<IChunkData> data;
	};

}