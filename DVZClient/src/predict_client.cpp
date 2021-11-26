#include "client/predict_client.hpp"
#include "client/voxel/clientchunkmanager.hpp"

#include "core/MovementUtil.hpp"
#include "core/voxel/VoxelCollision.hpp"

using namespace DVZ;

ClientPlayerState DVZ::predict_client_player_state(const Engine& engine, const MovementState& state, const Transformation& transform) {
	const auto& registry = engine.getRegistry();
	entt::entity player = engine.getPlayer();

	const auto& dir = registry.get<const Direction>(player);
	const auto& collider = registry.get<const VoxelCollider>(player);

	glm::vec3 vel = computePlayerVelocity(state, transform.rot, dir);
	
	const auto& chunkManager = engine.getChunkManager();
	auto getBlockFunc = [&chunkManager](const Voxel::WorldCoords& coords) -> Voxel::BlockType {
		return chunkManager.getBlock(coords);
	};

	Collision::AABB worldspace_aabb = collider.collider;
	worldspace_aabb.min += transform.pos;
	worldspace_aabb.max += transform.pos;

	auto broadphase = Voxel::broadphase(worldspace_aabb, vel, getBlockFunc);
	glm::vec3 new_vel = Voxel::sample_terrain_collision(transform.pos, vel, collider.collider, broadphase);
	vel = new_vel;

	Transformation new_transform = transform;
	new_transform.pos = transform.pos + vel;

	return ClientPlayerState{ new_transform, vel };
}