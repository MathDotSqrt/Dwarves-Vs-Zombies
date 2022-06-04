#include "client/voxel/ClientChunk.hpp"

using namespace DVZ::Voxel;

ClientChunk::ClientChunk(const ChunkCoords& coords) : IChunk(coords) {

}

bool ClientChunk::setBlock(const BlockCoords& coords, BlockType type) {
	if (IChunk::setBlock(coords, type)) {
		incrementRenderUpdateCount();
		return true;
	}
	return false;
}

bool ClientChunk::decompress(const CompressedChunk& compressed) {
	if (IChunk::decompress(compressed)) {
		incrementRenderUpdateCount();
		return true;
	}
	return false;
}

void ClientChunk::incrementRenderUpdateCount() {
	renderUpdateCount++;
}

i32 ClientChunk::getRenderUpdateCount() const {
	return renderUpdateCount;
}