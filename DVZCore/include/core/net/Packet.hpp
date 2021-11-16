#pragma once
#ifndef DVZ_PACKET_IDS_HPP
#define DVZ_PACKET_IDS_HPP

#include "core/common.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <string_view>
#include <cstddef>
#include <variant>

namespace DVZ::Net {

	enum class PacketID : u8 {
		ClientConnected,
		NetPlayerSpawned,
		PlayerPositionVel,
		Echo
	};

	struct EchoPacketData {
		constexpr static PacketID ID = PacketID::Echo;

		std::string message;
		template <class Archive>
		void serialize(Archive& ar) {
			ar(message);
		}
	};

	struct PlayerPositionVelPacketData {
		constexpr static PacketID ID = PacketID::PlayerPositionVel;

		glm::vec3 pos;
		glm::vec3 vel;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
		}
	};

	std::vector<char> serializePacketData(const EchoPacketData& data);
	std::vector<char> serializePacketData(const PlayerPositionVelPacketData& data);
	
	bool deserializePacketData(std::string_view, EchoPacketData& out);
	bool deserializePacketData(std::string_view, PlayerPositionVelPacketData& out);
	
}

#endif