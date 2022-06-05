#include "server/ServerNetSendSystem.hpp"
#include "server/Engine.hpp"
#include "server/net/EntitySnapshot.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/voxel/ServerChunkManager.hpp"

#include "server/ServerComponents.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"
#include "core/util/Timer.hpp"

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

using namespace DVZ;

ServerNetSendSystem::ServerNetSendSystem() {
	
}

void ServerNetSendSystem::init(Engine& engine) {

}

void ServerNetSendSystem::tick(Engine& engine) {
	Timer timer{ "ServerNetSendSystem::tick" };

	auto& registry = engine.getRegistry();
	auto& netManager = engine.getNetManager();
	auto& chunkManager = engine.getChunkManager();
	
	{
		Timer timer2{ "compute_snapshot" };
		netManager.setEntitySnapshot(std::make_shared<Net::EntitySnapshot>(engine));
	}

	Timer timer3{ "send_player" };
	auto view = registry.view<NetPlayer, Transformation>();
	for (entt::entity player : view) {

		HSteamNetConnection connection = netManager.getConnectionHandle(player);

		auto ack_number = netManager.shouldAckEntityInput(player);
		if (ack_number) {
			const auto& transform = view.get<Transformation>(player);
			Net::CB_PlayerPositionAckPacket packet{ transform.pos, ack_number.value() };
			netManager.sendMessage(packet, connection, false);
		}

		{
			Timer timer4{ "send_state" };
			Net::CB_EntitySnapshotDeltaPacket deltaPacket;
			deltaPacket.delta = netManager.getClientSnapshotDelta(connection);
			deltaPacket.server_time = engine.getTimeElapsed();
			netManager.sendMessage(deltaPacket, connection, false);
		}


		Timer timer5{ "send_chunk" };
		auto& netPlayer = view.get<NetPlayer>(player);
		if (netPlayer.shouldSend) {
			for (const Voxel::ChunkCoords& coords : netPlayer.unackedChunks) {
				const Voxel::CompressedChunk* compressed = chunkManager.getCompressedChunk(coords);
				if (compressed) {
					netManager.sendMessage(Net::CB_ChunkData{ *compressed }, connection, true);
				}
			}
			netPlayer.shouldSend = false;
		}
	}
	//TODO: after detecting user input, always send back the player position
	

}
