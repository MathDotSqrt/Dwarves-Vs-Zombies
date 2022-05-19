#pragma once
#ifndef DVZ_PACKET_IDS_HPP
#define DVZ_PACKET_IDS_HPP

#include "core/common.hpp"
#include "core/time.hpp"
#include "core/util/serialization.hpp"

#include "core/net/EntitySnapshotDelta.hpp"
#include "core/voxel/chunk.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string_view>
#include <cstddef>
#include <variant>

namespace DVZ::Net {

	//TODO: make seperate enum for clientbound vs serverbound packets
	enum class PacketID : u8 {
		SB_ClientJoin,				//Server Bound:	sends init info to the server
		SB_ClientDisconnected,		//Server Bound: Not a real packet, just sent to net recv system when client connection dies
		SB_PlayerPositionVel,		//Server Bound: position vel of client player
		SB_PlayerInput,				//Server Bound: input from client to simulate on server
		SB_AckEntitySnapshotDelta,	//Server Bound: ack entity snapshot delta

		CB_AssignNetID,				//Client Bound: gives client its server assigned ID
		CB_SyncSimulationClock,		//Client Bound: send server simulation time to client
		CB_SpawnPosition,			//Client Bound: specifies spawn position of client player
		CB_EntityPositionVel,		//Client Bound: position vel of entities/players 
		CB_EntitySnapshotDelta,     //Client Bound: sends game state delta from last ack number
		CB_PlayerPositionAck,		//Client Bound: auth position ack back to client for reconsiliation
		CB_PlayerJoin,				//Client Bound: notifies client a new player joined
		CB_ChunkData 				//Client Bound: compressed chunk data


	};

	struct SB_ClientJoinPacket {
		constexpr static PacketID packet = PacketID::SB_ClientJoin;

		std::string name;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(name);
		}
	};

	struct SB_PlayerPositionRotPacket {
		constexpr static PacketID packet = PacketID::SB_PlayerPositionVel;

		glm::vec3 pos;
		glm::quat rot;
		simulation_duration client_time;
		template <class Archive>
		void serialize(Archive& ar) {
			ar(pos.x, pos.y, pos.z, rot.w, rot.x, rot.y, rot.z, client_time);
		}
	};

	struct SB_PlayerInput {
		constexpr static PacketID packet = PacketID::SB_PlayerInput;
		glm::quat rot;
		simulation_duration client_time;

		i8 forward;
		i8 strafe;
		i8 fly;
		template <class Archive>
		void serialize(Archive& ar) {
			ar(rot.x, rot.y, rot.z, rot.w, client_time, forward, strafe, fly);
		}
	};

	struct SB_AckEntitySnapshoDelta {
		constexpr static PacketID packet = PacketID::SB_AckEntitySnapshotDelta;
		simulation_duration server_time;
		template <class Archive>
		void serialize(Archive& ar) {
			ar(server_time);
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

	struct CB_SyncSimulationClockPacket {
		constexpr static PacketID packet = PacketID::CB_SyncSimulationClock;

		simulation_duration server_time;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(server_time);
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

	struct CB_EntityPositionRotPacket {
		constexpr static PacketID packet = PacketID::CB_EntityPositionVel;

		entt::entity entity;
		glm::vec3 pos;
		glm::quat rot;
		simulation_duration server_time;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(entity, pos.x, pos.y, pos.z, rot.w, rot.x, rot.y, rot.z, server_time);
		}
	};

	struct CB_EntitySnapshotDeltaPacket {
		constexpr static PacketID packet = PacketID::CB_EntitySnapshotDelta;
		EntitySnapshotDelta delta;
		DVZ::simulation_duration server_time;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(delta, server_time);
		}
	};

	struct CB_PlayerPositionAckPacket{
		constexpr static PacketID packet = PacketID::CB_PlayerPositionAck;
		glm::vec3 pos;
		simulation_duration client_time;

		template <class Archive>
		void serialize(Archive& ar) {
			ar(pos.x, pos.y, pos.z, client_time);
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

	struct CB_ChunkData{
		constexpr static PacketID packet = PacketID::CB_ChunkData;

		Voxel::ChunkCoords coords;
		std::vector<Voxel::BlockType> compressed;

		template<class Archive>
		void serialize(Archive& ar){
			ar(coords.x, coords.y, coords.z, compressed);
		}
	};

	template<typename T>
	PacketID getPacketID() {
		return T::packet;
	}
	
	template<typename Packet>
	std::vector<char> serializePacketData(const Packet& data) {
		return serialize<>(data.packet, data);
	}

	template<typename Packet>
	bool deserializePacketData(std::string_view sv, Packet& out) {
		return deserialize<>(sv, out);
	}
}

#endif
