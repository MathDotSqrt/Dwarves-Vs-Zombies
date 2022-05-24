#ifndef DVZ_SERVER_COMPONENTS_HPP
#define DVZ_SERVER_COMPONENTS_HPP

#include "core/common.hpp"
#include "core/time.hpp"

#include "core/voxel/chunk.hpp"

#include <vector>

namespace DVZ {
	struct Debug {
		float timer;
	};

	struct NetPlayer {
		std::vector<Voxel::ChunkCoords> unackedChunks;
	};
}

#endif