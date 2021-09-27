#pragma once
#ifndef DVZ_BLOCK_HPP
#define DVZ_BLOCK_HPP

#include "core/common.hpp"

namespace DVZ::Voxel {
	struct Block {
		
	};

	enum class BlockType : u8{
		AIR = 0,
		GRASS,
		NUM_BLOCKS
	};
}

#endif