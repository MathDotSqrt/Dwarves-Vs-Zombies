#ifndef DVZ_IVOXEL_WORLD_HPP
#define DVZ_IVOXEL_WORLD_HPP

#include "core/voxel/IChunk.hpp"
#include <optional>

namespace DVZ::Voxel {

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
		virtual BlockType getBlock(const BlockCoords& coords) const = 0;
		virtual bool setBlock(const BlockCoords& coords, BlockType type) = 0;

		virtual IChunk* getChunk(const ChunkCoords& coords) = 0;
		virtual const IChunk* getChunk(const ChunkCoords& coords) const = 0;

		virtual const WorldCoords& getMinWorldBound() const = 0;
		virtual const WorldCoords& getMaxWorldBound() const = 0;

		std::optional<VoxelRaycastResult> raycast(const glm::vec3& origin, const glm::vec3& dir, float radius) const;
	};

}
#endif