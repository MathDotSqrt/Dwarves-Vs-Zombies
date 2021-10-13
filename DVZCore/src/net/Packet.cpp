#include "core/net/Packet.hpp"

#include <iostream>
#include <cereal/archives/binary.hpp>

using namespace DVZ::Net;

using bytestream = std::basic_stringstream<std::byte>;

std::vector<char> DVZ::Net::serializePacketData(const PlayerPositionVelPacketData& data) {
	std::stringstream ss;
	cereal::BinaryOutputArchive archive{ ss };

	archive(PacketID::PlayerPositionVel);
	archive(data);

	return std::vector<char>{std::istreambuf_iterator<char>{ss}, std::istreambuf_iterator<char>{}};
}