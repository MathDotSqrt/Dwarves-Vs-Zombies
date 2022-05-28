#include "server/ServerVoxelSystem.hpp"
#include "server/Engine.hpp"
#include "server/voxel/ServerChunkManager.hpp"

#include "core/CoreComponents.hpp"
#include "core/util/Timer.hpp"
#include "server/ServerComponents.hpp"

using namespace DVZ;

void ServerVoxelSystem::init(Engine& engine) {

}

void ServerVoxelSystem::tick(Engine& engine){
	Timer timer{"ServerVoxelSystem::tick"};

	auto& registry = engine.getRegistry();
	auto& manager = engine.getChunkManager();
	

	auto player_chunk_view = registry.view<NetPlayer, Transformation>();
	for (entt::entity id : player_chunk_view) {
		const auto& transform = player_chunk_view.get<Transformation>(id);
		auto& netPlayer = player_chunk_view.get<NetPlayer>(id);

		if (netPlayer.unackedChunks.size() == 0) {
			std::vector<Voxel::ChunkCoords> newUnackedChunks = manager.getUnackedChunks(id, transform.pos, NetPlayer::MAX_BUFFER);
			netPlayer.unackedChunks.insert(netPlayer.unackedChunks.end(), newUnackedChunks.begin(), newUnackedChunks.end());
			netPlayer.shouldSend = true;
		}
	}
}