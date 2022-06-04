#ifndef DVZ_CLIENT_CHUNK_HPP
#define DVZ_CLIENT_CHUNK_HPP

#include "core/voxel/IChunk.hpp"

namespace DVZ::Voxel {
	
	class ClientChunk : public IChunk {
	public:
		ClientChunk(const ChunkCoords& coords);

		bool setBlock(const BlockCoords& coords, BlockType type) override;

		bool decompress(const CompressedChunk& compressed) override;

		void incrementPX();
		void incrementNX();
		void incrementPZ();
		void incrementNZ();

		i32 getPX() const;
		i32 getNX() const;
		i32 getPZ() const;
		i32 getNZ() const;
	private:
		i32 pxRenderUpdateCount = 0;
		i32 nxRenderUpdateCount = 0;
		i32 pzRenderUpdateCount = 0;
		i32 nzRenderUpdateCount = 0;
	};

}

#endif 