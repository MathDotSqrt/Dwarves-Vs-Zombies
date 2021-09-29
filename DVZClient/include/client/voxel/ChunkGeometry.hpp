#pragma once
#ifndef DVZ_CHUNK_GEOMETRY_HPP
#define DVZ_CHUNK_GEOMETRY_HPP

#include "client/graphics/Attrib.hpp"
#include "client/graphics/Geometry.hpp"

#include <vector>

namespace DVZ::Voxel {
	class ChunkGeometry {
	private:
		//TODO look into 4 element storing normal
		using BlockPositionAttrib = Graphics::Attrib<POSITION_ATTRIB_LOCATION, glm::u8vec4>;
		using BlockNormalAttrib = Graphics::Attrib<NORMAL_ATTRIB_LOCATION, glm::i8vec4>;
		using BlockGeometry = Graphics::Geometry<BlockPositionAttrib, BlockNormalAttrib>;

	public:
		using BlockVertex = BlockGeometry::GeometryVertex;

		ChunkGeometry();

		void appendFace(
			const BlockVertex& v0,
			const BlockVertex& v1,
			const BlockVertex& v2,
			const BlockVertex& v3
		);

	private:
	};
}

#endif 