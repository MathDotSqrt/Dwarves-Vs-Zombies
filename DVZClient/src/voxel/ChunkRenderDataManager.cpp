#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/voxel/ClientChunkManager.hpp"
#include "client/util/util.hpp"

#include <spdlog/spdlog.h>
#include <algorithm>

using namespace DVZ::Voxel;

ChunkMesher::ChunkMesher() : blocks(std::make_unique<ChunkMesher::BlockStorage>()), coords{0}{
}

void ChunkMesher::loadChunkData(const ChunkNeighbors& n) {

	std::fill(blocks->begin(), blocks->end(), BlockType::AIR);
	assert(n.center);
	if (n.center) {
		coords = n.center->getChunkCoords();

		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bz = 0; bz < CHUNK_Z; bz++) {
				for (int bx = 0; bx < CHUNK_X; bx++) {
					getPaddedBlock(bx, by, bz) = n.center->getBlock(bx, by, bz);
				}
			}
		}
	}

	if (n.nz) {
		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bx = 0; bx < CHUNK_X; bx++) {
				getPaddedBlock(bx, by, CHUNK_Z) = n.nz->getBlock(bx, by, 0);
			}
		}
	}

	if (n.pz) {
		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bx = 0; bx < CHUNK_X; bx++) {
				getPaddedBlock(bx, by, -1) = n.pz->getBlock(bx, by, CHUNK_Z - 1);
			}
		}
	}

	if (n.nx) {
		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bz = 0; bz < CHUNK_Z; bz++) {
				getPaddedBlock(-1, by, bz) = n.nx->getBlock(CHUNK_X - 1, by, bz);
			}
		}
	}

	if (n.px) {
		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bz = 0; bz < CHUNK_Z; bz++) {
				getPaddedBlock(CHUNK_X, by, bz) = n.px->getBlock(0, by, bz);
			}
		}
	}
}

const ChunkVertexVector& ChunkMesher::meshChunk() {
	geometry.clear();
	
	for (int by = 0; by < CHUNK_Y; by++) {
		for (int bz = 0; bz < CHUNK_Z; bz++) {
			for (int bx = 0; bx < CHUNK_X; bx++) {
				BlockType block = getPaddedBlock(bx, by, bz);
				if (block == BlockType::AIR) {
					continue;
				}

				BlockFaceCullTags tags;
				tags.nx = getPaddedBlock(bx - 1, by, bz) != BlockType::AIR;
				tags.px = getPaddedBlock(bx + 1, by, bz) != BlockType::AIR;
				tags.ny = getPaddedBlock(bx, by - 1, bz) != BlockType::AIR;
				tags.py = getPaddedBlock(bx, by + 1, bz) != BlockType::AIR;
				tags.nz = getPaddedBlock(bx, by, bz - 1) != BlockType::AIR;
				tags.pz = getPaddedBlock(bx, by, bz + 1) != BlockType::AIR;
				
				BlockCoords coord(bx, by, bz);
				appendCubeGeometry(coord, tags);
			}
		}
	}

	return geometry;
}

void ChunkMesher::appendCubeGeometry(const BlockCoords& coord, BlockFaceCullTags tags) {
	BlockCoords p0{ coord.x, coord.y, coord.z + 1 };
	BlockCoords p1{ coord.x + 1, coord.y, coord.z + 1 };
	BlockCoords p2{ coord.x + 1, coord.y + 1, coord.z + 1 };
	BlockCoords p3{ coord.x, coord.y + 1, coord.z + 1 };

	BlockCoords p4{ coord.x + 1, coord.y, coord.z };
	BlockCoords p5{ coord.x, coord.y, coord.z };
	BlockCoords p6{ coord.x, coord.y + 1, coord.z };
	BlockCoords p7{ coord.x + 1, coord.y + 1, coord.z };

	glm::vec4 color{ .2, .7, .3, 1 };

	//Front
	if (!tags.pz) {
		glm::i8vec4 n{ 0, 0, 1, 0 };
		geometry.emplace_back(glm::u8vec4(p0, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p1, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p2, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p3, 0), n, color);
	}

	//Back
	if (!tags.nz) {
		glm::i8vec4 n{ 0, 0, -1, 0 };
		geometry.emplace_back(glm::u8vec4(p4, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p5, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p6, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p7, 0), n, color);
	}

	//Left
	if (!tags.nx) {
		glm::i8vec4 n{ -1, 0, 0, 0 };
		geometry.emplace_back(glm::u8vec4(p5, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p0, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p3, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p6, 0), n, color);
	}

	//Right
	if (!tags.px) {
		glm::i8vec4 n{ 1, 0, 0, 0 };
		geometry.emplace_back(glm::u8vec4(p1, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p4, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p7, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p2, 0), n, color);
	}

	//Top
	if (!tags.py) {
		glm::i8vec4 n{ 0, 1, 0, 0 };
		geometry.emplace_back(glm::u8vec4(p3, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p2, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p7, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p6, 0), n, color);
	}

	//Bottom
	if (!tags.ny) {
		glm::i8vec4 n{ 0, -1, 0, 0 };
		geometry.emplace_back(glm::u8vec4(p1, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p0, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p5, 0), n, color);
		geometry.emplace_back(glm::u8vec4(p4, 0), n, color);
	}
}

BlockType& ChunkMesher::getPaddedBlock(int bx, int by, int bz) {
	bx += 1;
	by += 1;
	bz += 1;

	//int index = (CHUNK_X * CHUNK_Z * by) + (bz * CHUNK_X) + bx;
	int index = bx + (PADDED_CHUNK_X * bz) + (PADDED_CHUNK_X * PADDED_CHUNK_Z * by);
	return (*blocks)[index];
}

void ChunkMesher::setCoords(const ChunkCoords& newCoords) {
	coords = newCoords;
}

const ChunkCoords& ChunkMesher::getCoords() const {
	return coords;
}

void ChunkRenderDataManager::bufferDirtyChunks(const ClientChunkManager& manager) {
	thread_local std::vector<ChunkCoords> coords;
	std::lock_guard<std::mutex> g{queue_mutex};

	playerCoords = manager.getPlayerChunkCoords();

	ChunkCoords render_radius{ ClientChunkManager::RENDER_RADIUS - 1, 0, ClientChunkManager::RENDER_RADIUS - 1 };

	coords.clear();
	Util::iterate_volume(playerCoords - render_radius, playerCoords + render_radius, [&](const ChunkCoords& coord) {
		coords.push_back(coord);
	});

	const auto chunk_distance = [&](const ChunkCoords& coord) {
		ChunkCoords delta = glm::abs(coord - playerCoords);
		return glm::max(delta.x, delta.z);
	};
	std::sort(coords.begin(), coords.end(), [&](const ChunkCoords& left, const ChunkCoords& right) {
		return chunk_distance(left) > chunk_distance(right);
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
			queuedChunks.back().loadChunkData(neighbors);
			chunkMeshUpdateCountMap[coord] = chunk->getUpdateCount();
		}
	}

	Util::iterate_volume(playerCoords - render_radius, playerCoords + render_radius, [&](const ChunkCoords& coord) {
		if (queuedChunks.size() >= MAX_CHUNK_MESH_QUEUE) {
			return;
		}

		
	});

	

}

void ChunkRenderDataManager::meshChunks() {
	std::lock_guard<std::mutex> g{ queue_mutex };

	const auto chunk_distance = [&](const ChunkCoords& coord) {
		ChunkCoords delta = glm::abs(coord - playerCoords);
		return glm::max(delta.x, delta.z);
	};

	//Removing all queued chunks if out of render distance
	auto iter = std::remove_if(queuedChunks.begin(), queuedChunks.end(), [&](const ChunkMesher& chunk) {
		return chunk_distance(chunk.getCoords()) > (ClientChunkManager::RENDER_RADIUS - 1);
	});
	std::move(iter, queuedChunks.end(), std::back_inserter(mesherPool));
	queuedChunks.erase(iter, queuedChunks.end());

	std::sort(queuedChunks.begin(), queuedChunks.end(), [&](const ChunkMesher& left, const ChunkMesher& right) {
		return chunk_distance(left.getCoords()) > chunk_distance(right.getCoords());
	});

	//Removing all ChunkRenderData if out of render distance
	auto iterRender = std::remove_if(renderableChunks.begin(), renderableChunks.end(), [&](const ChunkRenderData& chunk) {
		return chunk_distance(chunk.getCoords()) > (ClientChunkManager::RENDER_RADIUS - 1);
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

const std::vector<ChunkRenderData>& ChunkRenderDataManager::getRenderableChunks() const {
	return renderableChunks;
}