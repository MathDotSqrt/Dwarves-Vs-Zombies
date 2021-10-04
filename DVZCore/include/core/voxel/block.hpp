#pragma once
#ifndef DVZ_BLOCK_HPP
#define DVZ_BLOCK_HPP

#include "core/common.hpp"

#include <glm/glm.hpp>
#include <array>

namespace DVZ::Voxel {
	constexpr float BLOCK_WIDTH = 1.0f;

	using TexIndex = u8;
	struct BlockTexIndices {
		BlockTexIndices(TexIndex all);
		BlockTexIndices(TexIndex top, TexIndex side, TexIndex bottom);
		BlockTexIndices(TexIndex px, TexIndex nx, TexIndex py, TexIndex ny, TexIndex pz, TexIndex nz);

		TexIndex px;
		TexIndex nx;
		TexIndex py;
		TexIndex ny;
		TexIndex pz;
		TexIndex nz;
	};

	enum class BlockType : u8 {
		AIR = 0,
		GRASS,
		DIRT,
		SAND,
		NUM_BLOCKS
	};

	enum class MeshType : u8 {
		MESH_TYPE_NONE,
		MESH_TYPE_CUBE,
		MESH_TYPE_X
	};

	enum class OccludeType : u8{
		OCCLUDE_TYPE_ALL,
		OCCLUDE_TYPE_SELF,
		OCCLUDE_TYPE_NONE
	};

	struct BlockData {
		BlockType blockType;
		MeshType meshType;
		OccludeType occludeType;
		glm::vec3 color;
		BlockTexIndices textureCoords;

	private:
		
	};

	const std::array<BlockData, static_cast<u8>(BlockType::NUM_BLOCKS)> blockDataArray = {
		BlockData{
			BlockType::AIR,
			MeshType::MESH_TYPE_NONE,
			OccludeType::OCCLUDE_TYPE_NONE,
			glm::vec3(0),
			BlockTexIndices{14}
		},
		BlockData{
			BlockType::GRASS,
			MeshType::MESH_TYPE_CUBE,
			OccludeType::OCCLUDE_TYPE_ALL,
			glm::vec3(.1, .7, .2),
			BlockTexIndices{0, 3, 2}
		},
		BlockData{
			BlockType::DIRT,
			MeshType::MESH_TYPE_CUBE,
			OccludeType::OCCLUDE_TYPE_ALL,
			glm::vec3(.7, .3, 0),
			BlockTexIndices{2}
		},
		BlockData{
			BlockType::SAND,
			MeshType::MESH_TYPE_CUBE,
			OccludeType::OCCLUDE_TYPE_ALL,
			glm::vec3(.99, .9, .7),
			BlockTexIndices{176}
		}
	};

	inline const BlockData& getBlockData(BlockType type) {
		return blockDataArray[static_cast<u8>(type)];
	}

	
}

#endif