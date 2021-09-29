#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/voxel/ClientChunkManager.hpp"

using namespace DVZ::Voxel;

ChunkMesher::ChunkMesher() : blocks(std::make_unique<ChunkMesher::BlockStorage>()){
	
}

void ChunkRenderDataManager::bufferDirtyChunks(const ClientChunkManager& manager) {
	std::lock_guard<std::mutex> g{queue_mutex};
	for (const Chunk& chunk : manager.getChunks()) {
		const ChunkCoords& coords = chunk.getChunkCoords();
		int updateCount = chunk.getUpdateCount();
		
		if (chunkMeshUpdateCountMap[coords] != updateCount) {
			//queuedChunks();
		}
	}
}