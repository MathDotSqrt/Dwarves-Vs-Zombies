#pragma once
#ifndef DVZ_CLIENT_CHUNK_MANAGER_HPP
#define DVZ_CLIENT_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"

#include <vector>

namespace DVZ::Voxel {
	class ClientChunkManager {

	public:
		ClientChunkManager();

		constexpr static const ChunkIndex RENDER_RADIUS = 3;
		void updatePlayerPosition(const glm::vec3& pos);

	private:
		ChunkCoords playerChunkCoords;
		bool hasChanged = true;

		std::vector<Chunk> chunks;
	};
}

#endif 