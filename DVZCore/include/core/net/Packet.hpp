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
		SB_ClientJoin,				//Server Bound:	sends init info to the server
		SB_PlayerPositionVel,		//Server Bound: position vel of client player
		
		CB_AssignNetID,				//Client Bound: gives client its server assigned ID
		CB_SpawnPosition,			//Client Bound: specifies spawn position of client player
		CB_EntityPositionVel,		//Client Bound: position vel of entities/players 
		CB_PlayerJoin				//Client Bound: notifies client a new player joined
	};

	struct SB_ClientJoinPacket {
		constexpr static PacketID packet = PacketID::SB_ClientJoin;

		std::string name;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(name);
		}
	};

	struct SB_PlayerPositionVel {
		constexpr static PacketID packet = PacketID::SB_PlayerPositionVel;

		glm::vec3 pos;
		glm::vec3 vel;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
		}
	};

	struct CB_AssignNetIDPacket {
		constexpr static PacketID packet = PacketID::CB_AssignNetID;

		entt::entity id;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(id);
		}
	};

	struct CB_SpawnPositionPacket {
		constexpr static PacketID packet = PacketID::CB_SpawnPosition;

		glm::vec3 pos;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(pos.x, pos.y, pos.z);
		}
	};

	struct CB_EntityPositionVelPacket {
		constexpr static PacketID packet = PacketID::CB_EntityPositionVel;

		entt::entity entity;
		glm::vec3 pos;
		glm::vec3 vel;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(entity, pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
		}
	};

	struct CB_PlayerJoinPacket {
		constexpr static PacketID packet = PacketID::CB_PlayerJoin;

		entt::entity server_id;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(server_id);
		}
	};

	template<typename T>
	PacketID getPacketID() {
		return T::packet;
	}

	std::vector<char> serializePacketData(const SB_ClientJoinPacket& data);
	std::vector<char> serializePacketData(const SB_PlayerPositionVel& data);

	std::vector<char> serializePacketData(const CB_AssignNetIDPacket& data);
	std::vector<char> serializePacketData(const CB_SpawnPositionPacket& data);
	std::vector<char> serializePacketData(const CB_EntityPositionVelPacket& data);
	std::vector<char> serializePacketData(const CB_PlayerJoinPacket& data);
	
	bool deserializePacketData(std::string_view, SB_ClientJoinPacket& out);
	bool deserializePacketData(std::string_view, SB_PlayerPositionVel& out);

	bool deserializePacketData(std::string_view, CB_AssignNetIDPacket& out);
	bool deserializePacketData(std::string_view, CB_SpawnPositionPacket& out);
	bool deserializePacketData(std::string_view, CB_EntityPositionVelPacket& out);
	bool deserializePacketData(std::string_view, CB_PlayerJoinPacket& out);
	
}

#endif