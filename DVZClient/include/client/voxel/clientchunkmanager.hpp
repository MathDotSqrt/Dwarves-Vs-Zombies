#pragma once
#ifndef DVZ_CLIENT_CHUNK_MANAGER_HPP
#define DVZ_CLIENT_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"

namespace DVZ::Voxel {
	class ClientChunkManager {

	public:
		void updatePlayerPosition(const glm::vec3& pos);

		ChunkCoords playerChunkCoords;
		bool hasChanged = true;
	};
}

#endif 