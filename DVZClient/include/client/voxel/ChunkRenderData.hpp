#pragma once
#ifndef DVZ_CHUNK_RENDER_DATA_HPP
#define DVZ_CHUNK_RENDER_DATA_HPP

#include "core/voxel/chunk.hpp"
#include "client/graphics/VAO.hpp"
#include "client/graphics/VBO.hpp"
#include "client/graphics/Geometry.hpp"
#include "client/graphics/Attrib.hpp"

#include <memory>
#include <vector>
#include <tuple>

namespace DVZ::Voxel {
	using BlockPositionAttrib = Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::vec<4, BlockIndex>>;
	using BlockNormalAttrib = Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::i8vec4>;
	using BlockColorAttrib = Graphics::Attrib<COLOR_ATTRIB_LOCATION, glm::vec4>;
	using BlockTextureCoordAttrib = Graphics::Attrib<TEXCOORD_ATTRIB_LOCATION, glm::u8vec4>;
	using BlockVertex = Graphics::Geometry<BlockPositionAttrib, BlockNormalAttrib, BlockColorAttrib, BlockTextureCoordAttrib>::GeometryVertex;


	using ChunkVertexVector = std::vector<BlockVertex>;


	class ChunkRenderData {
	public:

		using EBOIndexType = u32;
		ChunkRenderData(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz);
		ChunkRenderData(const ChunkCoords& coords);
		//
		void bufferGeometry(const ChunkVertexVector& v);

		const ChunkCoords& getCoords() const;
		const Graphics::VAO& getVAO() const;
		const Graphics::VBO& getEBO() const;

	private:

		struct QuadIndices {
			EBOIndexType indices[6];
			QuadIndices(EBOIndexType startIndex);
		};

		static std::vector<QuadIndices> quads;
		static std::unique_ptr<Graphics::VBO> master_ebo;

		void expandMasterEBO(size_t newQuadSize);

		ChunkCoords coords;
		Graphics::VAO vao;
		Graphics::VBO vbo;

		size_t indexCount;
	};
}

#endif