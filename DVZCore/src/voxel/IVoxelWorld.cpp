#include "core/voxel/IVoxelWorld.hpp"
#include "core/util/math.hpp"

#include <cmath>

using namespace DVZ::Voxel;

float intbound(float s, float ds) {
	if (ds < 0) {
		s = -s;
		ds = -ds;
	}
	s = fmodf(s, 1.0f);
	return (1 - s) / ds;
}

ChunkNeighbors IVoxelWorld::getChunkNeighbors(const ChunkCoords& coords) const {
	constexpr ChunkCoords X{ 1, 0, 0 };
	constexpr ChunkCoords Y{ 0, 1, 0 };
	constexpr ChunkCoords Z{ 0, 0, 1 };

	ChunkNeighbors n;
	n.center = getChunk(coords);
	n.nx = getChunk(coords - X);
	n.px = getChunk(coords + X);
	n.nz = getChunk(coords - Z);
	n.pz = getChunk(coords + Z);

	return n;
}

const WorldCoords& IVoxelWorld::getMinWorldBound() const {
	return minBound;
}

const WorldCoords& IVoxelWorld::getMaxWorldBound() const {
	return maxBound;
}

std::optional<VoxelRaycastResult> IVoxelWorld::raycast(const glm::vec3& origin, const glm::vec3& dir, float radius) const {
	constexpr float epsilon = 0.001f;

	float dir_length = glm::length(dir);

	if (dir_length < epsilon) {
		return {};
	}

	const glm::ivec3 step = glm::sign(dir);

	const glm::vec3 delta = glm::vec3(step) / dir;

	radius /= dir_length;

	const WorldCoords& maxBound = getMaxWorldBound();
	const WorldCoords& minBound = getMinWorldBound();

	const auto not_past_bounds = [&](const WorldCoords& coords) -> bool {
		return
			(step.x > 0 ? coords.x < maxBound.x : coords.x >= minBound.x) &&
			(step.y > 0 ? coords.y < maxBound.y : coords.y >= minBound.y) &&
			(step.z > 0 ? coords.z < maxBound.z : coords.z >= minBound.z);
	};

	glm::vec3 tmax{ intbound(origin.x, dir.x), intbound(origin.y, dir.y), intbound(origin.z, dir.z) };
	WorldCoords coords = Voxel::toWorldCoords(origin);
	glm::vec3 normal{ 0 };

	while (not_past_bounds(coords)) {
		const BlockType type = getBlock(coords);
		if (type != BlockType::AIR) {
			return VoxelRaycastResult{ coords, normal, type };
		}

		const auto update_component = [&](int component) -> bool {
			if (tmax[component] > radius) {
				return true;
			}

			coords[component] += step[component];
			tmax[component] += delta[component];
			normal = glm::vec3{ 0 };
			normal[component] = (float)(-step[component]);

			return false;
		};

		const auto index = DVZ::compMinIndex(tmax);
		if (update_component(index)) {
			break;
		}
	}

	return {};
}