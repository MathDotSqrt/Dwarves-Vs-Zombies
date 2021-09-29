#pragma once
#ifndef DVZ_CHUNK_RENDER_DATA_HPP
#define DVZ_CHUNK_RENDER_DATA_HPP

#include "core/voxel/chunk.hpp"
#include "client/graphics/VAO.hpp"
#include "client/graphics/VBO.hpp"

#include <memory>
#include <vector>

namespace DVZ::Voxel {
	class ChunkRenderData {
	public:
		using EBOIndexType = u32;

		ChunkRenderData(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz);

		//
		void bufferGeometry();

		ChunkIndex getCX() const;
		ChunkIndex getCY() const;
		ChunkIndex getCZ() const;

	private:

		struct QuadIndices {
			EBOIndexType indices[6];
			QuadIndices(EBOIndexType startIndex);
		};

		static std::vector<QuadIndices> quads;
		static std::unique_ptr<Graphics::VBO> master_ebo;

		void expandMasterEBO(EBOIndexType newQuadSize);

		ChunkIndex cx;
		ChunkIndex cy;
		ChunkIndex cz;
		Graphics::VAO vao;
		Graphics::VBO vbo;

		size_t indexCount;
	};
}

#endif