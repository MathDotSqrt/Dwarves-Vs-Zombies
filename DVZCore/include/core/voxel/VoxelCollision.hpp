#pragma once
#ifndef DVZ_VOXEL_COLLISION_HPP
#define DVZ_VOXEL_COLLISION_HPP

#include "core/voxel/chunk.hpp"
#include <functional>
#include <vector>

namespace DVZ::Voxel {
	using GetBlockFunc = std::function<BlockType(const WorldCoords&)>;

	std::vector<std::pair<WorldCoords, BlockType>> broadphase();
}

#endif