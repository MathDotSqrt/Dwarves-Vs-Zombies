#pragma once
#ifndef DVZ_CLIENT_CHUNK_MANAGER_HPP
#define DVZ_CLIENT_CHUNK_MANAGER_HPP

#include "core/voxel/chunk.hpp"
#include "client/util/util.hpp"
#include "core/voxel/IVoxelWorld.hpp"

#include <vector>
#include <optional>
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

		inline bool isSurrounded() const {
			return center != nullptr
				&& px != nullptr
				&& nx != nullptr
				&& pz != nullptr
				&& nz != nullptr;
		}
	};

	//struct VoxelRaycastResult {
	//	WorldCoords coords;
	//	glm::vec3 normal;
	//	BlockType type;
	//};

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

		std::optional<VoxelRaycastResult> raycast(const glm::vec3& origin, const glm::vec3& dir, float radius) const;

		bool addCompressedChunk(CompressedChunk&& compressed_chunk);
		//const std::vector<Chunk>& getChunks() const;
	private:
		Chunk* getChunkOrAllocate(const ChunkCoords& coords);

		Chunk allocateChunk(const ChunkCoords& coords);
		void deallocateChunk(Chunk&& chunk);

		bool setBlockInternal(const WorldCoords& coords, BlockType block);
		
		void decompressChunks();
		void queueChunksToDelete();
		void queueChunksToGenerate();

		ChunkCoords playerChunkCoords;
		bool hasChanged = true;

		std::unordered_map<ChunkCoords, Chunk> chunks;
		std::unordered_map<ChunkCoords, CompressedChunk> compressedChunks;
		std::vector<Chunk> chunksPool;

		WorldCoords minBound;
		WorldCoords maxBound;
	};
}

#endif 