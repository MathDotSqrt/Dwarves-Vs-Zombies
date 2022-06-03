#pragma once
#ifndef DVZ_VOXEL_COLLISION_HPP
#define DVZ_VOXEL_COLLISION_HPP

#include "core/voxel/VoxelConstants.hpp"
#include "core/voxel/block.hpp"
#include "core/collision/AABB.hpp"
#include <functional>
#include <vector>

namespace DVZ::Voxel {
	using GetBlockFunc = std::function<BlockType(const WorldCoords&)>;

	std::vector<std::pair<WorldCoords, BlockType>> broadphase(
		const Collision::AABB& worldspace_aabb, const glm::vec3& vel, const GetBlockFunc& getBlock
	);
	
	glm::vec3 sample_terrain_collision(
		const glm::vec3& pos,
		const glm::vec3& vel,
		const Collision::AABB& aabb,
		std::vector<std::pair<WorldCoords, BlockType>>& broadphase
	);
}

#endif