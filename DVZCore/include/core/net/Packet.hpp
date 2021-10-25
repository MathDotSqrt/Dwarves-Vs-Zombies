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

	struct PlayerPositionVelPacketData {
		glm::vec3 pos;
		glm::vec3 vel;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
		}
	};

	using PacketDataVariant = std::variant<PlayerPositionVelPacketData>;

	std::vector<char> serializePacketData(const PlayerPositionVelPacketData& data);

	PacketID deserializePacketData(std::string_view );
	
}

#endif