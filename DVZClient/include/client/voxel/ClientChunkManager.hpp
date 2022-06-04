#pragma once
#ifndef DVZ_CLIENT_CHUNK_MANAGER_HPP
#define DVZ_CLIENT_CHUNK_MANAGER_HPP

#include "client/voxel/ClientChunk.hpp"
#include "client/util/util.hpp"

#include "core/voxel/IVoxelWorld.hpp"

#include <vector>
#include <optional>
#include <unordered_map>

namespace DVZ::Voxel {


	class ClientChunkManager : public IVoxelWorld {
	public:
#ifdef DVZ_CLIENT_DEBUG
		constexpr static const ChunkIndex RENDER_RADIUS = 10;
#else
		constexpr static const ChunkIndex RENDER_RADIUS = 20;
#endif

		ClientChunkManager();


		BlockType getBlock(const WorldCoords& coords) const override;
		bool setBlock(const WorldCoords& coords, BlockType type) override;

		IChunk* getChunk(const ChunkCoords& coords) override;
		const IChunk* getChunk(const ChunkCoords& coords) const override;

		ClientChunk* getClientChunk(const ChunkCoords& coords);
		const ClientChunk* getClientChunk(const ChunkCoords& coords) const;

		void updatePlayerPosition(const glm::vec3& pos);

		const ChunkCoords& getPlayerChunkCoords() const;


		bool addCompressedChunk(CompressedChunk&& compressed_chunk);
		//const std::vector<Chunk>& getChunks() const;
	private:
		IChunk* getChunkOrAllocate(const ChunkCoords& coords);

		bool setBlockInternal(const WorldCoords& coords, BlockType block);
		
		void decompressChunks();

		ChunkCoords playerChunkCoords;
		bool hasChanged = true;

		std::unordered_map<ChunkCoords, ClientChunk> chunks;
		std::unordered_map<ChunkCoords, CompressedChunk> compressedChunks;
	};
}

#endif 