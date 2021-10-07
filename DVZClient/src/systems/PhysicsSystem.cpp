#include "client/systems/PhysicsSystem.hpp"
#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/voxel/clientchunkmanager.hpp"

#include "core/CoreComponents.hpp"
#include "core/voxel/VoxelCollision.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void PhysicsSystem::init(Engine& engine) {

}

void PhysicsSystem::gameTick(Engine& engine) {
	Voxel::ClientChunkManager& manager = engine.getChunkManager();
	auto getBlockFunc = [&manager](const Voxel::WorldCoords& coords) -> Voxel::BlockType{
		return manager.getBlock(coords);
	};

	manager.setBlock(Voxel::WorldCoords{0, 0, 0}, Voxel::BlockType::SAND);

	auto& registry = engine.getRegistry();
	auto view = registry.view<Transformation, Velocity, VoxelCollider>();
	view.each([&](Transformation& trans, Velocity& vel, VoxelCollider& collider) {
		Collision::AABB worldspace_aabb = collider.collider;
		worldspace_aabb.min += trans.pos;
		worldspace_aabb.max += trans.pos;

		auto broadphase = Voxel::broadphase(worldspace_aabb, vel, getBlockFunc);
		if (broadphase.size() > 0)
			spdlog::debug("broads in atlanta");
		glm::vec3 new_vel = Voxel::sample_terrain_collision(trans.pos, vel, collider.collider, broadphase);


		vel = new_vel;
		//vel.y = 0;
	});


	auto vel_view = registry.view<DVZ::Transformation, DVZ::Velocity>();
	for (const auto& e : vel_view) {
		auto& trans = vel_view.get<DVZ::Transformation>(e);
		auto& vel = vel_view.get<DVZ::Velocity>(e);
		trans.pos = trans.pos + vel;
	}
}