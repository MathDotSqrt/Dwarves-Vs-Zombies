#include "core/net/Packet.hpp"
#include "core/util/serialization.hpp"

#include <iostream>

using namespace DVZ;
using namespace DVZ::Net;

std::vector<char> DVZ::Net::serializePacketData(const SB_ClientJoinPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const SB_PlayerPositionRotPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const SB_PlayerInput& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_AssignNetIDPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_SyncSimulationClockPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_SpawnPositionPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_EntityPositionRotPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_PlayerPositionAckPacket& data) {
	return serialize<>(data.packet, data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_PlayerJoinPacket& data) {
	return serialize<>(data.packet, data);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, SB_ClientJoinPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, SB_PlayerPositionRotPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, SB_PlayerInput& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_AssignNetIDPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_SyncSimulationClockPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_SpawnPositionPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_EntityPositionRotPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_PlayerPositionAckPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_PlayerJoinPacket& out) {
	return deserialize<>(sv, out);
}