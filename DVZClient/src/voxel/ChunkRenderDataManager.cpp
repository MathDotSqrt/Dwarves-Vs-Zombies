#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/voxel/ClientChunkManager.hpp"
#include "client/util/util.hpp"

#include <spdlog/spdlog.h>
#include <algorithm>

using namespace DVZ::Voxel;

void ChunkRenderDataManager::bufferDirtyChunks(const ClientChunkManager& manager) {
	thread_local std::vector<ChunkCoords> coords;
	std::lock_guard<std::mutex> g{queue_mutex};

	playerCoords = manager.getPlayerChunkCoords();

	ChunkCoords render_radius{ ClientChunkManager::RENDER_RADIUS - 1, 0, ClientChunkManager::RENDER_RADIUS - 1 };

	coords.clear();
	Util::iterate_volume(playerCoords - render_radius, playerCoords + render_radius, [&](const ChunkCoords& coord) {
		coords.push_back(coord);
	});

	std::sort(coords.begin(), coords.end(), [&](const ChunkCoords& left, const ChunkCoords& right) {
		return chunkDistance(left) > chunkDistance(right);
	});

	while (queuedChunks.size() < MAX_CHUNK_MESH_QUEUE && coords.size() > 0) {
		ChunkCoords coord = coords.back();
		coords.pop_back();

		const Chunk* chunk = manager.getChunk(coord);
		if (chunk && chunkMeshUpdateCountMap[coord] != chunk->getUpdateCount()) {
			ChunkNeighbors neighbors = manager.getChunkNeighbors(coord);

			if (mesherPool.size() > 0) {
				auto& newMesher = mesherPool.back();
				assert(newMesher.blocks);
				queuedChunks.emplace_back(std::move(newMesher));
				assert(queuedChunks.back().blocks);

				mesherPool.pop_back();
			}
			else {
				queuedChunks.emplace_back();
				assert(queuedChunks.back().blocks);

			}
			assert(queuedChunks.size() > 0);
			assert(queuedChunks.back().blocks);
			queuedChunks.back().loadChunkData(neighbors);
			chunkMeshUpdateCountMap[coord] = chunk->getUpdateCount();
		}
	}
}

void ChunkRenderDataManager::meshChunks() {
	std::lock_guard<std::mutex> g{ queue_mutex };

	//Removing all queued chunks if out of render distance
	//auto iter = std::remove_if(queuedChunks.begin(), queuedChunks.end(), [&](const ChunkMesher& chunk) {
	//	assert(chunk.blocks);
	//	return chunkDistance(chunk.getCoords()) > (ClientChunkManager::RENDER_RADIUS - 1);
	//});
	//std::move(iter, queuedChunks.end(), std::back_inserter(mesherPool));
	//queuedChunks.erase(iter, queuedChunks.end());

	std::sort(queuedChunks.begin(), queuedChunks.end(), [&](const ChunkMesher& left, const ChunkMesher& right) {
		return chunkDistance(left.getCoords()) > chunkDistance(right.getCoords());
	});

	//Removing all ChunkRenderData if out of render distance
	auto iterRender = std::remove_if(renderableChunks.begin(), renderableChunks.end(), [&](const ChunkRenderData& chunk) {
		return chunkDistance(chunk.getCoords()) > (ClientChunkManager::RENDER_RADIUS - 1);
	});
	renderableChunks.erase(iterRender, renderableChunks.end());

	if (queuedChunks.size() > 0) {
		ChunkMesher& mesher = queuedChunks.back();
		renderableChunks.emplace_back(mesher.getCoords());
		const auto& geometry = mesher.meshChunk();
		renderableChunks.back().bufferGeometry(geometry);

		mesherPool.emplace_back(std::move(mesher));
		queuedChunks.pop_back();
		spdlog::debug("Generated Mesh: [{}]", geometry.size());
	}
}

int ChunkRenderDataManager::chunkDistance(const ChunkCoords& coords) const {
	ChunkCoords delta = glm::abs(coords - playerCoords);
	return glm::max(delta.x, delta.z);
}

const std::vector<ChunkRenderData>& ChunkRenderDataManager::getRenderableChunks() const {
	return renderableChunks;
}