#include "client/voxel/ChunkRenderData.hpp"


#include <numeric>
#include <spdlog/spdlog.h>

using namespace DVZ::Voxel;

ChunkRenderData::QuadIndices::QuadIndices(EBOIndexType startIndex) {
	indices[0] = startIndex + 0;
	indices[1] = startIndex + 1;
	indices[2] = startIndex + 2;
	indices[3] = startIndex + 0;
	indices[4] = startIndex + 2;
	indices[5] = startIndex + 3;
}

ChunkRenderData::ChunkRenderData(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz) : vbo(Graphics::VBO::BufferType::ARRAY_BUFFER) {
	coords = ChunkCoords{ cx, cy, cz };
	this->indexCount = 0;

	if (master_ebo == nullptr) {
		master_ebo = std::make_unique<Graphics::VBO>(Graphics::VBO::BufferType::ELEMENT_ARRAY_BUFFER);
		expandMasterEBO(6000);
	}

	vao.bind();
	vbo.bind();
	vao.addVertexAttribPtr<BlockPositionAttrib, BlockNormalAttrib, BlockColorAttrib>();
	vbo.unbind();
	master_ebo->bind();
	vao.unbind();
	master_ebo->unbind();
}

ChunkRenderData::ChunkRenderData(const ChunkCoords& coords) : ChunkRenderData(coords.x, coords.y, coords.z) {
	
}

void ChunkRenderData::bufferGeometry(const ChunkVertexVector& v) {
	assert(v.size() % 4 == 0);

	size_t numQuads = v.size() / 4;
	size_t maxQuadSize = ChunkRenderData::quads.size();
	if (numQuads > maxQuadSize) {
		numQuads = 2 * numQuads;
		expandMasterEBO(numQuads);
	}

	vbo.bind();
	vbo.bufferData(v);
	vbo.unbind();

	//6 indices per quad
	this->indexCount = numQuads * 6;
}

const ChunkCoords& ChunkRenderData::getCoords() const {
	return coords;
}

const DVZ::Graphics::VAO& ChunkRenderData::getVAO() const {
	return vao;
}

const DVZ::Graphics::VBO& ChunkRenderData::getEBO() const {
	return *ChunkRenderData::master_ebo;
}

void ChunkRenderData::expandMasterEBO(size_t newQuadSize) {
	spdlog::info("Master Chunk EBO Resized: [{}] Quads", newQuadSize);

	quads.reserve(newQuadSize);
	for (size_t i = quads.size(); i < newQuadSize; i++) {
		//Assert to prevent overflow
		assert((i * 4) <= (std::numeric_limits<EBOIndexType>::max() - 3));

		//There are 4 vertices for each quad
		//Emplace back is faster, no extra copy (I think lol)
		//calls constructor for QuadIndices(i * 4);
		EBOIndexType startIndex = (EBOIndexType)(i * 4);
		quads.emplace_back(startIndex);
	}

	master_ebo->bind();
	master_ebo->bufferData(quads);
	master_ebo->unbind();
}

std::unique_ptr<DVZ::Graphics::VBO> ChunkRenderData::master_ebo = nullptr;
std::vector<ChunkRenderData::QuadIndices> ChunkRenderData::quads;