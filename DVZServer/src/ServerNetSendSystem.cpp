#include "server/ServerNetSendSystem.hpp"
#include "server/Engine.hpp"
#include "server/net/EntitySnapshot.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/voxel/ServerChunkManager.hpp"

#include "server/ServerComponents.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

#include <entt/entt.hpp>

using namespace DVZ;

ServerNetSendSystem::ServerNetSendSystem() {
	
}

void ServerNetSendSystem::init(Engine& engine) {

}

void ServerNetSendSystem::tick(Engine& engine) {
	auto& registry = engine.getRegistry();
	auto& netManager = engine.getNetManager();
	
	netManager.setEntitySnapshot(std::make_shared<Net::EntitySnapshot>(engine));

	auto view = registry.view<NetPlayer, Transformation>();

	for (entt::entity player : view) {
		HSteamNetConnection connection = netManager.getConnectionHandle(player);

		auto ack_number = netManager.shouldAckEntityInput(player);
		if (ack_number) {
			const auto& transform = view.get<Transformation>(player);
			Net::CB_PlayerPositionAckPacket packet{ transform.pos, ack_number.value() };
			netManager.sendMessage(packet, connection, false);
		}

		Net::CB_EntitySnapshotDeltaPacket deltaPacket;
		deltaPacket.delta = netManager.getClientSnapshotDelta(connection);
		deltaPacket.server_time = engine.getTimeElapsed();
		netManager.sendMessage(deltaPacket, connection, false);
	}
	//TODO: after detecting user input, always send back the player position
	
	const auto& chunkManager = engine.getChunkManager();

}
