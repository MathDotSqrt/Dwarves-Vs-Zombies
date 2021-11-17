#pragma once
#ifndef DVZ_PACKET_IDS_HPP
#define DVZ_PACKET_IDS_HPP

#include "core/common.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <string_view>
#include <cstddef>
#include <variant>

namespace DVZ::Net {

	enum class PacketID : u8 {
		ClientConnected,
		Echo,
		PlayerPositionVel,
		NetPlayerSpawned
	};

	struct ClientConnectedPacketData {
		std::string name;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(name);
		}
	};

	struct EchoPacketData {
		std::string message;
		template <class Archive>
		void serialize(Archive& ar) {
			ar(message);
		}
	};

	struct PlayerPositionVelPacketData {
		entt::entity entity;
		glm::vec3 pos;
		glm::vec3 vel;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(entity, pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
		}
	};

	struct NetPlayerSpawned {
		entt::entity server_id;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(server_id);
		}
	};

	std::vector<char> serializePacketData(const ClientConnectedPacketData& data);
	std::vector<char> serializePacketData(const EchoPacketData& data);
	std::vector<char> serializePacketData(const PlayerPositionVelPacketData& data);
	std::vector<char> serializePacketData(const NetPlayerSpawned& data);
	
	bool deserializePacketData(std::string_view, ClientConnectedPacketData& out);
	bool deserializePacketData(std::string_view, EchoPacketData& out);
	bool deserializePacketData(std::string_view, PlayerPositionVelPacketData& out);
	bool deserializePacketData(std::string_view, NetPlayerSpawned& out);
	
}

#endif