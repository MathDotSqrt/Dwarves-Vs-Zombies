#include "core/net/Packet.hpp"
#include "core/util/view_streambuf.hpp"

#include <iostream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

using namespace DVZ::Net;

template<typename T>
std::vector<char> serialize(PacketID id, const T& packet) {
	std::stringstream ss;
	cereal::BinaryOutputArchive archive{ ss };

	archive(id);
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

std::vector<char> DVZ::Net::serializePacketData(const EchoPacketData& data) {
	return serialize<>(PacketID::Echo, data);
}

std::vector<char> DVZ::Net::serializePacketData(const PlayerPositionVelPacketData& data) {
	return serialize<>(PacketID::PlayerPositionVel, data);
}

std::vector<char> DVZ::Net::serializePacketData(const NetPlayerSpawned& data) {
	return serialize<>(PacketID::NetPlayerSpawned, data);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, EchoPacketData& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, PlayerPositionVelPacketData& out) {
	return deserialize<>(sv, out);
}

bool DVZ::Net::deserializePacketData(std::string_view sv, NetPlayerSpawned& out) {
	return deserialize<>(sv, out);
}