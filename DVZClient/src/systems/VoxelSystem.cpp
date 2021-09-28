#include "client/systems/VoxelSystem.hpp"

#include "client/engine.hpp"
#include "client/components.hpp"
#include "client/voxel/ClientChunkManager.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void VoxelSystem::init(Engine& engine) {
	spdlog::info("Initalized [VoxelSystem]");
}

void VoxelSystem::gameTick(Engine& engine) {
	entt::registry& registry = engine.getRegistry();
	Voxel::ClientChunkManager& manager = engine.getChunkManager();
	
	entt::entity player = engine.getPlayer();
	Transformation transform = registry.get<Transformation>(player);

	manager.updatePlayerPosition(transform.pos);




}