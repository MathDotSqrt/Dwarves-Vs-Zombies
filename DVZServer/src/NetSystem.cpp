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

	netManager.sendToAllMessage(Net::CB_SyncSimulationClockPacket{engine.getTimeElapsed()}, false);

	for (entt::entity entity : netplayer_view) {
		const auto& trans = netplayer_view.get<Transformation>(entity);
		auto& network = netplayer_view.get<Network>(entity);
		auto& netplayer = netplayer_view.get<NetPlayer>(entity);

		netplayer.timer -= simulation_duration{ 1 };
		if (netplayer.timer == simulation_duration{ 0 }) {
			Net::CB_EntityPositionRotPacket data;
			data.entity = entity;
			data.pos = trans.pos;
			data.rot = trans.rot;
			data.server_time = engine.getTimeElapsed();
			netManager.sendToAllMessage(data, netManager.getConnectionHandle(entity), false);
			network.last_pos = trans.pos;

			netplayer.timer = NetPlayer::DELAY;
		}
		
	}

	auto view = registry.view<Transformation, Velocity, Network>(entt::exclude<NetPlayer>);
	for (entt::entity entity : view) {
		const auto& trans = view.get<Transformation>(entity);
		auto& network = view.get<Network>(entity);

		//if (glm::distance2(trans.pos, network.last_pos) > .01f || trans.rot != network.last_rot) {
			Net::CB_EntityPositionRotPacket data;
			data.entity = entity;
			data.pos = trans.pos;
			data.rot = trans.rot;
			data.server_time = engine.getTimeElapsed();
			//data.vel = vel;

			netManager.sendToAllMessage(data, false);
			network.last_pos = trans.pos;
		//}
	}
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

		auto& registry = engine.getRegistry();
		auto& netManager = engine.getNetManager();

		entt::entity clientID = registry.create();
		glm::vec3 spawn_pos = glm::vec3(0, 100, 0);

		netManager.addEntityConnectionMapping(clientID, conn);
		netManager.sendMessage(Net::CB_AssignNetIDPacket{clientID}, conn, true);
		netManager.sendMessage(Net::CB_SpawnPositionPacket{ spawn_pos }, conn, true);
		netManager.sendToAllMessage(Net::CB_PlayerJoinPacket{ clientID }, conn, true);
		
		auto view = registry.view<Network>();
		for (entt::entity entity : view) {
			netManager.sendMessage(Net::CB_PlayerJoinPacket{ entity }, conn, true);
		}

		registry.emplace<Transformation>(clientID, spawn_pos);
		registry.emplace<Velocity>(clientID, glm::vec3{ 0 });
		registry.emplace<Network>(clientID);
		registry.emplace<NetPlayer>(clientID);

		
	}
}

void NetSystem::onPlayerPositionVel(Engine& engine, std::string_view data, HSteamNetConnection conn) {
	Net::SB_PlayerPositionRotPacket packet;
	if (Net::deserializePacketData(data, packet)) {
		auto& registry = engine.getRegistry();
		auto& netManager = engine.getNetManager();
		entt::entity player = netManager.getEntity(conn);

		if (player != entt::null && netManager.shouldAcceptClientMovement(player, packet.client_time)) {
			auto& transform = registry.get<Transformation>(player);
			transform.pos = packet.pos;
			transform.rot = packet.rot;
		}
	}
}

