#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/voxel/ClientChunkManager.hpp"


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

const ChunkCoords& ChunkMesher::getCoords() const {
	return coords;
}

void ChunkRenderDataManager::bufferDirtyChunks(const ClientChunkManager& manager) {
	std::lock_guard<std::mutex> g{queue_mutex};

	ChunkCoords center = manager.getPlayerChunkCoords();
	const Chunk* chunk = manager.getChunk(center);

	if (chunk && chunkMeshUpdateCountMap[center] != chunk->getUpdateCount()) {
		ChunkNeighbors neighbors = manager.getChunkNeighbors(center);
		queuedChunks.emplace_back();
		queuedChunks.back().loadChunkData(neighbors);
		chunkMeshUpdateCountMap[center] = chunk->getUpdateCount();
	}

}

void ChunkRenderDataManager::meshChunks() {
	std::lock_guard<std::mutex> g{ queue_mutex };

	if (queuedChunks.size() > 0) {
		renderableChunks.emplace_back(queuedChunks.back().getCoords());
		const auto& geometry = queuedChunks.back().meshChunk();
		renderableChunks.back().bufferGeometry(geometry);
		
		
		spdlog::debug("Generated Mesh: [{}]", geometry.size());
		queuedChunks.pop_back();
	}
}

const std::vector<ChunkRenderData>& ChunkRenderDataManager::getRenderableChunks() const {
	return renderableChunks;
}