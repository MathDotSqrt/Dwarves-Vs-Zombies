#include "server/ServerVoxelSystem.hpp"
#include "server/Engine.hpp"
#include "server/voxel/ServerChunkManager.hpp"

#include "core/CoreComponents.hpp"
#include "server/ServerComponents.hpp"

using namespace DVZ;

void ServerVoxelSystem::init(Engine& engine) {

}

void ServerVoxelSystem::tick(Engine& engine){
	auto& registry = engine.getRegistry();
	auto& manager = engine.getChunkManager();
	

	auto player_chunk_view = registry.view<NetPlayer, Transformation>();
	for (entt::entity id : player_chunk_view) {
		const auto& transform = player_chunk_view.get<Transformation>(id);
		auto& netPlayer = player_chunk_view.get<NetPlayer>(id);

		manager.updatePlayer(id, transform.pos);
		
		auto chunks = manager.popPlayerUnackedChunks(id, 8 - netPlayer.unackedChunks.size());
		netPlayer.unackedChunks.insert(netPlayer.unackedChunks.end(), chunks.rbegin(), chunks.rend());
	}
}