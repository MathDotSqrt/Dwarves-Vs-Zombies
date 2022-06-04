#include "client/voxel/ClientChunk.hpp"

using namespace DVZ::Voxel;

ClientChunk::ClientChunk(const ChunkCoords& coords) : IChunk(coords) {

}

bool ClientChunk::setBlock(const BlockCoords& coords, BlockType type) {
	if (IChunk::setBlock(coords, type)) {
		if (coords.x == MIN_BLOCK_COORDS.x) {
			incrementNX();
		}
		if (coords.x == MAX_BLOCK_COORDS.x) {
			incrementPX();
		}
		if (coords.z == MIN_BLOCK_COORDS.z) {
			incrementNZ();
		}
		if (coords.z == MAX_BLOCK_COORDS.z) {
			incrementPZ();
		}
		return true;
	}
	return false;
}

bool ClientChunk::decompress(const CompressedChunk& compressed) {
	if (IChunk::decompress(compressed)) {
		incrementPX();
		incrementNX();
		incrementPZ();
		incrementNZ();

		return true;
	}
	return false;
}

void ClientChunk::incrementPX(){
	pxRenderUpdateCount += 1;
}

void ClientChunk::incrementNX() {
	nxRenderUpdateCount += 1;
}

void ClientChunk::incrementPZ() {
	pzRenderUpdateCount += 1;
}

void ClientChunk::incrementNZ() {
	nzRenderUpdateCount += 1;
}

i32 ClientChunk::getPX() const {
	return pxRenderUpdateCount;
}

i32 ClientChunk::getNX() const {
	return nxRenderUpdateCount;
}

i32 ClientChunk::getPZ() const {
	return pzRenderUpdateCount;
}

i32 ClientChunk::getNZ() const {
	return nzRenderUpdateCount;
}