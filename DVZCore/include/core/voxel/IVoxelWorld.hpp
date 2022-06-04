#ifndef DVZ_IVOXEL_WORLD_HPP
#define DVZ_IVOXEL_WORLD_HPP

#include "core/voxel/IChunk.hpp"
#include <optional>

namespace DVZ::Voxel {

	struct ChunkNeighbors {
		const IChunk* center = nullptr;
		const IChunk* nx = nullptr;
		const IChunk* px = nullptr;
		const IChunk* nz = nullptr;
		const IChunk* pz = nullptr;

		inline bool isSurrounded() const {
			return center != nullptr
				&& px != nullptr
				&& nx != nullptr
				&& pz != nullptr
				&& nz != nullptr;
		}
	};

	struct VoxelRaycastResult {
		WorldCoords coords;
		glm::vec3 normal;
		BlockType type;
	};

	struct CollisionResult {
		glm::vec3 new_vel;
	};

	class IVoxelWorld {
	public:
		virtual BlockType getBlock(const WorldCoords& coords) const = 0;
		virtual bool setBlock(const WorldCoords& coords, BlockType type) = 0;

		virtual IChunk* getChunk(const ChunkCoords& coords) = 0;
		virtual const IChunk* getChunk(const ChunkCoords& coords) const = 0;

		virtual ChunkNeighbors getChunkNeighbors(const ChunkCoords& coords) const;
		virtual ChunkNeighbors getChunkNeighbors(const IChunk* chunk) const;

		virtual const WorldCoords& getMinWorldBound() const;
		virtual const WorldCoords& getMaxWorldBound() const;

		std::optional<VoxelRaycastResult> raycast(const glm::vec3& origin, const glm::vec3& dir, float radius) const;

	protected:
		WorldCoords minBound = WorldCoords{0};
		WorldCoords maxBound = WorldCoords{0};
	};

}
#endif