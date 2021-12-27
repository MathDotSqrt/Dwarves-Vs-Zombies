#include "client/systems/VoxelSystem.hpp"

#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/voxel/ClientChunkManager.hpp"
#include "client/window.hpp"

#include "core/CoreComponents.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void VoxelSystem::init(Engine& engine) {

	spdlog::info("Initalized [VoxelSystem]");
}

void VoxelSystem::gameTick(Engine& engine) {
	entt::registry& registry = engine.getRegistry();
	Voxel::ClientChunkManager& manager = engine.getChunkManager();
	
	entt::entity player = engine.getPlayer();
	const Transformation& transform = registry.get<Transformation>(player);
	const Direction& dir = registry.get<Direction>(player);

	manager.updatePlayerPosition(transform.pos);


	const auto window = Window::getInstance();

	bool is_break = window.isClick(Window::Mouse::LEFT_CLICK);
	bool is_place = window.isClick(Window::Mouse::RIGHT_CLICK);

	glm::vec3 origin = transform.pos;
	glm::vec3 forward = transform.rot * dir.forward;
	if (is_place) {
		const auto result = manager.raycast(origin, forward, 100.0f);
		if (result) {
			Voxel::WorldCoords new_coord = result->coords + Voxel::WorldCoords{ result->normal };
			manager.setBlock(new_coord, Voxel::BlockType::SAND);
		}
	}
	else if (is_break) {
		const auto result = manager.raycast(origin, forward, 100.0f);
		if (result) {
			manager.setBlock(result->coords, Voxel::BlockType::AIR);
		}
	}
}