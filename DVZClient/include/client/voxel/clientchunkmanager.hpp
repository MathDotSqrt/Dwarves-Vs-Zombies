#pragma once
#ifndef DVZ_CLIENT_CHUNK_MANAGER_HPP
#define DVZ_CLIENT_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"
#include "client/util/util.hpp"
#include <vector>
#include <unordered_map>

namespace DVZ::Voxel {
	struct ChunkNeighbors {
		const Chunk* center = nullptr;
		const Chunk* nx = nullptr;
		const Chunk* px = nullptr;
		const Chunk* nz = nullptr;
		const Chunk* pz = nullptr;
		const Chunk* ny = nullptr;
		const Chunk* py = nullptr;
	};

	class ClientChunkManager {
	public:
#ifdef DVZ_CLIENT_DEBUG
		constexpr static const ChunkIndex RENDER_RADIUS = 10;
#else
		constexpr static const ChunkIndex RENDER_RADIUS = 20;
#endif
		ClientChunkManager();


		template<typename FUNC>
		void iterateChunks(const ChunkCoords& min, const ChunkCoords& max, FUNC&) {
			//Util::iterate_volume(min, max, []() {
			//
			//});
		}

		void updatePlayerPosition(const glm::vec3& pos);

		const Chunk* getChunk(const ChunkCoords& coords) const;
		const Chunk* getChunk(ChunkIndex cx, ChunkIndex cy, ChunkIndex cz) const;

		Chunk* getChunk(const ChunkCoords& coords);

		ChunkNeighbors getChunkNeighbors(const ChunkCoords& coords) const;

		const ChunkCoords& getPlayerChunkCoords() const;

		BlockType getBlock(const WorldCoords& coords) const;
		bool setBlock(const WorldCoords& coords, BlockType block);
		//const std::vector<Chunk>& getChunks() const;
	private:
		void queueChunksToDelete();
		void queueChunksToGenerate();

		ChunkCoords playerChunkCoords;
		bool hasChanged = true;

		std::unordered_map<ChunkCoords, Chunk> chunks;
		std::vector<Chunk> chunksPool;


	};
}

#endif 