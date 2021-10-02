#pragma once
#ifndef DVZ_BLOCK_HPP
#define DVZ_BLOCK_HPP

#include "core/common.hpp"

#include <glm/glm.hpp>
#include <array>

namespace DVZ::Voxel {
	constexpr float BLOCK_WIDTH = 1.0f;

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
	};

	const std::array<BlockData, static_cast<u8>(BlockType::NUM_BLOCKS)> blockDataArray = {
		BlockData{
			BlockType::AIR,
			MeshType::MESH_TYPE_NONE,
			OccludeType::OCCLUDE_TYPE_NONE,
			glm::vec3(0)
		},
		BlockData{
			BlockType::GRASS,
			MeshType::MESH_TYPE_CUBE,
			OccludeType::OCCLUDE_TYPE_ALL,
			glm::vec3(.1, .7, .2)
		},
		BlockData{
			BlockType::DIRT,
			MeshType::MESH_TYPE_CUBE,
			OccludeType::OCCLUDE_TYPE_ALL,
			glm::vec3(.7, .3, 0)
		},
		BlockData{
			BlockType::SAND,
			MeshType::MESH_TYPE_CUBE,
			OccludeType::OCCLUDE_TYPE_ALL,
			glm::vec3(.99, .9, .7)
		}
	};

	inline const BlockData& getBlockData(BlockType type) {
		return blockDataArray[static_cast<u8>(type)];
	}

	
}

#endif