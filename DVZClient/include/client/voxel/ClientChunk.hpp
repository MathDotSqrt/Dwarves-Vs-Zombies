#ifndef DVZ_CLIENT_CHUNK_HPP
#define DVZ_CLIENT_CHUNK_HPP

#include "core/voxel/IChunk.hpp"

namespace DVZ::Voxel {
	
	class ClientChunk : public IChunk {
	public:
		ClientChunk(const ChunkCoords& coords);

		bool setBlock(const BlockCoords& coords, BlockType type) override;

		bool decompress(const CompressedChunk& compressed) override;

		void incrementRenderUpdateCount();

		i32 getRenderUpdateCount() const;

	private:
		i32 renderUpdateCount = 0;
		
	};

}

#endif 