#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/voxel/ClientChunkManager.hpp"
#include "client/util/util.hpp"

#include "core/util/Frustum.hpp"

#include <spdlog/spdlog.h>
#include <algorithm>

using namespace DVZ::Voxel;

void ChunkRenderDataManager::bufferDirtyChunks(const Frustum& frustum, const ClientChunkManager& manager) {
	thread_local std::vector<ChunkCoords> coords;
	std::lock_guard<std::mutex> g{queue_mutex};

	playerCoords = manager.getPlayerChunkCoords();

	ChunkCoords render_radius{ ClientChunkManager::RENDER_RADIUS - 1, 0, ClientChunkManager::RENDER_RADIUS - 1 };

	coords.clear();
	Util::iterate_volume(playerCoords - render_radius, playerCoords + render_radius, [&](const ChunkCoords& coord) {
		Collision::AABB aabb = Voxel::getChunkAABB(coord);
		if (frustum.intersects(aabb)) {
			coords.push_back(coord);
		}
		else {
			//spdlog::info("failed");
		}
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
				queuedChunks.emplace_back(std::move(newMesher));

				mesherPool.pop_back();
			}
			else {
				queuedChunks.emplace_back();

			}
			assert(queuedChunks.size() > 0);
			queuedChunks.back().loadChunkData(neighbors);
			chunkMeshUpdateCountMap[coord] = chunk->getUpdateCount();
		}
	}
}

void ChunkRenderDataManager::update() {
	std::lock_guard<std::mutex> g{ queue_mutex };


	cullFarChunks();
	meshChunks();
}

void ChunkRenderDataManager::clearRenderData() {
	std::lock_guard<std::mutex> g{ queue_mutex };
	queuedChunks.clear();
	renderableChunks.clear();
	chunkMeshUpdateCountMap.clear();
}

void ChunkRenderDataManager::cullFarChunks() {
	//Removing all queued chunks if out of render distance
	//auto iter = std::remove_if(queuedChunks.begin(), queuedChunks.end(), [&](const ChunkMesher& chunk) {
	//	assert(chunk.blocks);
	//	return chunkDistance(chunk.getCoords()) > (ClientChunkManager::RENDER_RADIUS - 1);
	//});
	//std::move(iter, queuedChunks.end(), std::back_inserter(mesherPool));
	//queuedChunks.erase(iter, queuedChunks.end());

	//Removing all ChunkRenderData if out of render distance
	auto iterRender = std::partition(renderableChunks.begin(), renderableChunks.end(), [&](const ChunkRenderData& chunk) {
		return chunkDistance(chunk.getCoords()) <= (ClientChunkManager::RENDER_RADIUS - 1);
	});



	std::for_each(iterRender, renderableChunks.end(), [&](const ChunkRenderData& data) {
		chunkMeshUpdateCountMap.erase(data.getCoords());
	});
	renderableChunks.erase(iterRender, renderableChunks.end());
}

void ChunkRenderDataManager::meshChunks() {
	launchMesherThreads();
	bufferMeshedChunks();
}

void ChunkRenderDataManager::launchMesherThreads() {
	std::sort(queuedChunks.begin(), queuedChunks.end(), [&](const ChunkMesher& left, const ChunkMesher& right) {
		return chunkDistance(left.getCoords()) > chunkDistance(right.getCoords());
	});

	while (queuedChunks.size() > 0 && futureChunkGeometries.size() < MAX_THREADS) {
		futureChunkGeometries.emplace_back(std::async(std::launch::async, &ChunkRenderDataManager::meshChunkAsync, this, std::move(queuedChunks.back())));
		queuedChunks.pop_back();
	}
}

void ChunkRenderDataManager::bufferMeshedChunks() {
	auto iter = std::partition(futureChunkGeometries.begin(), futureChunkGeometries.end(), [](const std::future<ChunkMesher>& future) {
		return !future.valid() || future.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
	});

	std::for_each(iter, futureChunkGeometries.end(), [&](std::future<ChunkMesher>& future) {
		assert(future.valid());
		ChunkMesher mesher = future.get();

		if (mesher.getGeometry().size() > 0) {
			renderableChunks.emplace_back(mesher.getCoords());
			renderableChunks.back().bufferGeometry(mesher.getGeometry());
		}
		mesherPool.emplace_back(std::move(mesher));
	});

	futureChunkGeometries.erase(iter, futureChunkGeometries.end());
}

ChunkMesher ChunkRenderDataManager::meshChunkAsync(ChunkMesher&& mesher) {
	mesher.meshChunk();
	return std::move(mesher);
}

int ChunkRenderDataManager::chunkDistance(const ChunkCoords& coords) const {
	ChunkCoords delta = glm::abs(coords - playerCoords);
	return glm::max(delta.x, delta.z);
}

const std::vector<ChunkRenderData>& ChunkRenderDataManager::getRenderableChunks() const {
	return renderableChunks;
}