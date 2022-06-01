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

	class IVoxelWorld {
	public:
		virtual IChunk* getChunk(const ChunkCoords& coords) = 0;
		virtual const IChunk* getChunk(const ChunkCoords& coords) const = 0;

		virtual std::optional<VoxelRaycastResult> raycast(const glm::vec3& origin, const glm::vec3& dir, float radius) const = 0;

	};

}
#endif