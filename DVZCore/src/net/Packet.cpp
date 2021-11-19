#include "core/net/Packet.hpp"
#include "core/util/view_streambuf.hpp"

#include <iostream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

using namespace DVZ::Net;

template<typename T>
std::vector<char> serialize(const T& packet) {
	std::stringstream ss;
	cereal::BinaryOutputArchive archive{ ss };

	archive(T::packet);
	archive(packet);

	return std::vector<char>{std::istreambuf_iterator<char>{ss}, std::istreambuf_iterator<char>{}};
}

template<typename T>
bool deserialize(std::string_view sv, T& packet) {
	DVZ::view_istream<char> istream{ sv };
	cereal::BinaryInputArchive archive{ istream };
	archive(packet);
	return true;
}

std::vector<char> DVZ::Net::serializePacketData(const SB_ClientJoinPacket& data) {
	return serialize<>(data);
}

std::vector<char> DVZ::Net::serializePacketData(const SB_PlayerPositionVel& data) {
	return serialize<>(data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_AssignNetIDPacket& data) {
	return serialize<>(data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_SpawnPositionPacket& data) {
	return serialize<>(data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_EntityPositionVelPacket& data) {
	return serialize<>(data);
}

std::vector<char> DVZ::Net::serializePacketData(const CB_PlayerJoinPacket& data) {
	return serialize<>(data);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, SB_ClientJoinPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, SB_PlayerPositionVel& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_AssignNetIDPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_SpawnPositionPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_EntityPositionVelPacket& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, CB_PlayerJoinPacket& out) {
	return deserialize<>(sv, out);
}