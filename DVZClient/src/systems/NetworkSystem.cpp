#include "client/systems/NetworkSystem.hpp"
#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/net/NetClientManager.hpp"


#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

NetworkSystem::NetworkSystem() {
	
}

void NetworkSystem::init(Engine& engine) {

}

void NetworkSystem::gameTick(Engine& engine) {
	using namespace Net;

	Net::NetClientManager& netManager = engine.getNetManager();

	netManager.poll([&](std::string_view sv) {
		this->onMessage(engine, sv);
	});

	
}

void NetworkSystem::onMessage(Engine& engine, std::string_view data) {
	using namespace Net;

	if (data.size() == 0) {
		spdlog::warn("Packet with zero size recieved");
		return;
	}

	PacketID packetID = static_cast<PacketID>(data[0]);
	data.remove_prefix(1);

	switch (packetID) {
	case PacketID::Echo:
		onEchoPacket(engine, data);
		break;
	case PacketID::PlayerPositionVel:
		onPlayerPosVelPacket(engine, data);
		break;
	case PacketID::NetPlayerSpawned:
		onNetPlayerSpawned(engine, data);
		break;
	default:
		break;
	}
}

void NetworkSystem::onEchoPacket(Engine& engine, std::string_view data) {
	using namespace Net;
	EchoPacketData echo;
	if (deserializePacketData(data, echo)) {
		spdlog::info("Echo: {}", echo.message);
	}
}

void NetworkSystem::onPlayerPosVelPacket(Engine& engine, std::string_view data) {
	using namespace Net;
	PlayerPositionVelPacketData posvel;
	if (deserializePacketData(data, posvel)) {
		
	}
}

void NetworkSystem::onNetPlayerSpawned(Engine& engine, std::string_view data) {
	using namespace Net;
	using namespace entt;

	NetPlayerSpawned spawn;
	if (deserializePacketData(data, spawn)) {
		auto& registry = engine.getRegistry();
		entt::entity netplayer = registry.create();
		auto& netEntityMap = engine.getNetEntityMap();
		
		netEntityMap[spawn.server_id] = netplayer;

		registry.emplace<Transformation>(netplayer, glm::vec3(0, 20, -10));
		registry.emplace<Velocity>(netplayer, glm::vec3{0});
		registry.emplace<MovementState>(netplayer);
		registry.emplace<Direction>(netplayer);
		registry.emplace<VoxelCollider>(netplayer, Collision::AABB{ glm::vec3(-.3, -1.5, -.3), glm::vec3(.3, .5, .3) });
		registry.emplace<Renderable>(netplayer, "cube"_hs);
	}
}