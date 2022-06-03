#ifndef DVZ_SERVER_COMPONENTS_HPP
#define DVZ_SERVER_COMPONENTS_HPP

#include "core/common.hpp"
#include "core/time.hpp"

#include "core/voxel/VoxelConstants.hpp"

#include <vector>

namespace DVZ {
	struct Debug {
		float timer;
	};

	struct NetPlayer {
		constexpr static size_t MAX_BUFFER = 8;
		std::vector<Voxel::ChunkCoords> unackedChunks;
		bool shouldSend = false;
	};
}

#endif