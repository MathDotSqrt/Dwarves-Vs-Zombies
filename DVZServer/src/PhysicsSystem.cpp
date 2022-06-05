#include "server/PhysicsSystem.hpp"
#include "server/Engine.hpp"
#include "core/CoreComponents.hpp"
#include "core/MovementUtil.hpp"
#include "server/ServerComponents.hpp"
#include "server/voxel/ServerChunkManager.hpp"
#include "core/voxel/VoxelCollision.hpp"
#include "core/util/Timer.hpp"

#include <random>
#include <spdlog/spdlog.h>

using namespace DVZ;

PhysicsSystem::PhysicsSystem() {

}

void PhysicsSystem::init(Engine& engine) {
	
}

void PhysicsSystem::tick(Engine& engine) {

	static std::mt19937 rng{123};

	Timer timer{"PhysicsSystem::tick"};

	auto& registry = engine.getRegistry();
	
	/* User Input */
	auto movement_view = registry.view<MovementState, Velocity, Transformation, Direction>();
	movement_view.each([](MovementState& state, Velocity& vel, Transformation& transform, Direction& dir) {
		vel = computePlayerVelocity(state, transform.rot, dir);
		state.forward = 0;
		state.strafe = 0;
		state.fly = 0;
	});

	auto getBlockFunc = [&](const Voxel::WorldCoords& coords) -> Voxel::BlockType {
		Voxel::ServerChunkManager& manager = engine.getChunkManager();
		return manager.getBlock(coords);
	};

	auto chunk_collider_view = registry.view<Transformation, Velocity, VoxelCollider>();
	chunk_collider_view.each([&](Transformation& trans, Velocity& vel, VoxelCollider& collider) {
		Collision::AABB worldspace_aabb = collider.collider;
		worldspace_aabb.min += trans.pos;
		worldspace_aabb.max += trans.pos;

		auto broadphase = Voxel::broadphase(worldspace_aabb, vel, getBlockFunc);
		glm::vec3 new_vel = Voxel::sample_terrain_collision(trans.pos, vel, collider.collider, broadphase);

		vel = new_vel;
		//vel.y = 0;
	});

	//if (std::uniform_int_distribution<i32>(0, 20)(rng) == 0) {
	//if (true) {
	//	for (int i = 0; i < 5; i++) {
	//		std::uniform_int_distribution<Voxel::WorldIndex> U(0, Voxel::CHUNK_X);
	//		std::uniform_int_distribution<Voxel::WorldIndex> U2(0, Voxel::CHUNK_Y);

	//		Voxel::WorldCoords coords{ U(rng), U2(rng), U(rng) };
	//		engine.getChunkManager().setBlock(coords, Voxel::BlockType::AIR);

	//	}
	//}

	auto debug_view = registry.view<Velocity, Debug>();

	debug_view.each([](Velocity& vel, Debug& debug) {
		debug.timer += .05f;
		vel.y = glm::sin(debug.timer) / 6;
		vel.x = glm::cos(debug.timer) / 6;
	});

	auto view = registry.view<Transformation, Velocity>();
	view.each([](Transformation& transform, Velocity& vel) {
		transform.pos = transform.pos + vel;
	});

}