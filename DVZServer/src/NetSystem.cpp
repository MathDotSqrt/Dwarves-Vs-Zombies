#include "server/NetSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/Engine.hpp"
#include "server/ServerComponents.hpp"

#include "core/CoreComponents.hpp"
#include "core/net/Packet.hpp"

#include <glm/gtx/norm.hpp>
#include <spdlog/spdlog.h>

using namespace DVZ;

NetSystem::NetSystem(Engine& engine) {
	auto& registry = engine.getRegistry();

	entt::entity test = registry.create();
	registry.emplace<Transformation>(test, glm::vec3{0, 100, 0});
	registry.emplace<Velocity>(test);
	registry.emplace<Debug>(test, 0);
	registry.emplace<Network>(test);
}

void NetSystem::tick(Engine& engine) {
	using namespace Net;

	Net::NetServerManager& netManager = engine.getNetManager();
	netManager.poll([&](std::string_view data, HSteamNetConnection connection) {
		onMessage(engine, data, connection);
	});

	auto& registry = engine.getRegistry();
	auto netplayer_view = registry.view<Transformation, Network, NetPlayer>();

}

void NetSystem::onMessage(Engine& engine, std::string_view data, HSteamNetConnection connection) {
	using namespace Net;

	if (data.size() == 0) {
		spdlog::error("Empty message from [{}]", connection);
		return;
	}

	PacketID id{ static_cast<PacketID>(data[0]) };

	data.remove_prefix(1);

	switch (id) {
	case PacketID::SB_ClientJoin:
		onClientJoin(engine, data, connection);
		break;
	case PacketID::SB_PlayerPositionVel:
		onPlayerPositionVel(engine, data, connection);
		break;
	default:
		spdlog::error("Message with invalid ID [{}] from [{}]", static_cast<std::byte>(data[0]), connection);
		break;
	}
}

void NetSystem::onClientJoin(Engine& engine, std::string_view data, HSteamNetConnection conn){
	Net::SB_ClientJoinPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		spdlog::info("Welcome: {}", packet.name);

	}
}

void NetSystem::onPlayerPositionVel(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_PlayerPositionRotPacket packet;
	if (Net::deserializePacketData(data, packet)) {

	}
}

