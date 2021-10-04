#include "client/voxel/ChunkMesher.hpp"
#include "client/voxel/ClientChunkManager.hpp"


using namespace DVZ::Voxel;


ChunkMesher::ChunkMesher() : blocks(std::make_unique<ChunkMesher::BlockStorage>()), coords{ 0 }{
}

void ChunkMesher::loadChunkData(const ChunkNeighbors& n) {
	assert(blocks);

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

	if (n.pz) {
		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bx = 0; bx < CHUNK_X; bx++) {
				getPaddedBlock(bx, by, CHUNK_Z) = n.pz->getBlock(bx, by, 0);
			}
		}
	}

	if (n.nz) {
		for (int by = 0; by < CHUNK_Y; by++) {
			for (int bx = 0; bx < CHUNK_X; bx++) {
				getPaddedBlock(bx, by, -1) = n.nz->getBlock(bx, by, CHUNK_Z - 1);
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

				const BlockData& blockData = getBlockData(block);

				BlockFaceCullTags tags;
				tags.nx = getPaddedBlock(bx - 1, by, bz) != BlockType::AIR;
				tags.px = getPaddedBlock(bx + 1, by, bz) != BlockType::AIR;
				tags.ny = getPaddedBlock(bx, by - 1, bz) != BlockType::AIR;
				tags.py = getPaddedBlock(bx, by + 1, bz) != BlockType::AIR;
				tags.nz = getPaddedBlock(bx, by, bz - 1) != BlockType::AIR;
				tags.pz = getPaddedBlock(bx, by, bz + 1) != BlockType::AIR;

				BlockCoords coord(bx, by, bz);
				appendCubeGeometry(coord, blockData, tags);
			}
		}
	}

	return geometry;
}

void ChunkMesher::appendCubeGeometry(const BlockCoords& coord, const BlockData& data, BlockFaceCullTags tags) {
	BlockCoords p0{ coord.x, coord.y, coord.z + 1 };
	BlockCoords p1{ coord.x + 1, coord.y, coord.z + 1 };
	BlockCoords p2{ coord.x + 1, coord.y + 1, coord.z + 1 };
	BlockCoords p3{ coord.x, coord.y + 1, coord.z + 1 };

	BlockCoords p4{ coord.x + 1, coord.y, coord.z };
	BlockCoords p5{ coord.x, coord.y, coord.z };
	BlockCoords p6{ coord.x, coord.y + 1, coord.z };
	BlockCoords p7{ coord.x + 1, coord.y + 1, coord.z };

	glm::vec4 color{ data.color, 1 };
	glm::u8vec2 uv0(0, 1);
	glm::u8vec2 uv1(1, 1);
	glm::u8vec2 uv2(1, 0);
	glm::u8vec2 uv3(0, 0);
	

	//Front
	if (!tags.pz) {
		glm::i8vec4 n{ 0, 0, 1, 0 };
		auto texIndex = data.textureCoords.pz;
		geometry.emplace_back(glm::u8vec4(p0, 0), n, color, glm::vec4(uv0, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p1, 0), n, color, glm::vec4(uv1, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p2, 0), n, color, glm::vec4(uv2, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p3, 0), n, color, glm::vec4(uv3, texIndex, 0));
	}

	//Back
	if (!tags.nz) {
		glm::i8vec4 n{ 0, 0, -1, 0 };
		auto texIndex = data.textureCoords.nz;
		geometry.emplace_back(glm::u8vec4(p4, 0), n, color, glm::vec4(uv0, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p5, 0), n, color, glm::vec4(uv1, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p6, 0), n, color, glm::vec4(uv2, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p7, 0), n, color, glm::vec4(uv3, texIndex, 0));
	}

	//Left
	if (!tags.nx) {
		glm::i8vec4 n{ -1, 0, 0, 0 };
		auto texIndex = data.textureCoords.nx;
		geometry.emplace_back(glm::u8vec4(p5, 0), n, color, glm::vec4(uv0, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p0, 0), n, color, glm::vec4(uv1, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p3, 0), n, color, glm::vec4(uv2, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p6, 0), n, color, glm::vec4(uv3, texIndex, 0));
	}

	//Right
	if (!tags.px) {
		glm::i8vec4 n{ 1, 0, 0, 0 };
		auto texIndex = data.textureCoords.px;
		geometry.emplace_back(glm::u8vec4(p1, 0), n, color, glm::vec4(uv0, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p4, 0), n, color, glm::vec4(uv1, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p7, 0), n, color, glm::vec4(uv2, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p2, 0), n, color, glm::vec4(uv3, texIndex, 0));
	}

	//Top
	if (!tags.py) {
		glm::i8vec4 n{ 0, 1, 0, 0 };
		auto texIndex = data.textureCoords.py;


		geometry.emplace_back(glm::u8vec4(p3, 0), n, color, glm::vec4(uv0, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p2, 0), n, color, glm::vec4(uv1, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p7, 0), n, color, glm::vec4(uv2, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p6, 0), n, color, glm::vec4(uv3, texIndex, 0));
	}

	//Bottom
	if (!tags.ny) {
		glm::i8vec4 n{ 0, -1, 0, 0 };
		auto texIndex = data.textureCoords.ny;
		geometry.emplace_back(glm::u8vec4(p1, 0), n, color, glm::vec4(uv0, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p0, 0), n, color, glm::vec4(uv1, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p5, 0), n, color, glm::vec4(uv2, texIndex, 0));
		geometry.emplace_back(glm::u8vec4(p4, 0), n, color, glm::vec4(uv3, texIndex, 0));
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

const ChunkVertexVector& ChunkMesher::getGeometry() const {
	return geometry;
}