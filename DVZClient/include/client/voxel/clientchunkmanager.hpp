#pragma once
#ifndef DVZ_CLIENT_CHUNK_MANAGER_HPP
#define DVZ_CLIENT_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"
#include "client/util/util.hpp"
#include <vector>


namespace DVZ::Voxel {
	class ClientChunkManager {
	public:
		constexpr static const ChunkIndex RENDER_RADIUS = 3;

		ClientChunkManager();


		template<typename FUNC>
		void iterateChunks(const ChunkCoords& min, const ChunkCoords& max, FUNC&) {
			//Util::iterate_volume(min, max, []() {
			//
			//});
		}

		void updatePlayerPosition(const glm::vec3& pos);

		const ChunkCoords& getPlayerChunkCoords() const;
		const std::vector<Chunk>& getChunks() const;
	private:
		ChunkCoords playerChunkCoords;
		bool hasChanged = true;

		std::vector<Chunk> chunks;
	};
}

#endif 