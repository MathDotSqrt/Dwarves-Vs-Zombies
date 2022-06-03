#ifndef DVZ_SERVER_CHUNK_MANAGER_HPP
#define DVZ_SERVER_CHUNK_MANAGER_HPP

#include "core/voxel/IChunk.hpp"
#include "core/voxel/IVoxelWorld.hpp"
#include <unordered_map>
#include <entt/entt.hpp>

namespace DVZ::Voxel {

	struct NetChunkState {
		std::unordered_map<const IChunk*, int> chunkAckMap;
	};

	class ServerChunkManager : public IVoxelWorld{
	public:
		ServerChunkManager();


		BlockType getBlock(const WorldCoords& coords) const override;
		bool setBlock(const WorldCoords& coords, BlockType block) override;
		
		IChunk* getChunk(const ChunkCoords& coords) override;
		const IChunk* getChunk(const ChunkCoords& coords) const override;

		void addPlayer(entt::entity id, const glm::vec3& world_position);
		void removePlayer(entt::entity id);
		void ackChunk(entt::entity id, ChunkCoords coord, int ack);
		std::vector<ChunkCoords> getUnackedChunks(entt::entity id, const glm::vec3& world_position, int N) const;
		const CompressedChunk* getCompressedChunk(const ChunkCoords& coords);

	private:
		constexpr static ChunkIndex WORLD_RADIUS = 30;
		constexpr static ChunkIndex RENDER_RADIUS = 15;


		std::unordered_map<ChunkCoords, IChunk> chunks;

		std::unordered_map<ChunkCoords, CompressedChunk> compressedChunksCache;

		std::unordered_map<entt::entity, NetChunkState> playerChunks;
	};
}

#endif