#ifndef DVZ_SERIALIZATION_HPP
#define DVZ_SERIALIZATION_HPP

#include "core/util/view_streambuf.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/variant.hpp>

namespace DVZ {
	template<typename ...T>
	std::vector<char> serialize(const T& ...values) {
		std::stringstream ss;
		cereal::BinaryOutputArchive archive{ ss };

		volatile std::initializer_list<bool> _{ (archive(values), false)... };

		return std::vector<char>{std::istreambuf_iterator<char>{ss}, std::istreambuf_iterator<char>{}};
	}

	template<typename T>
	bool deserialize(std::string_view sv, T& packet) {
		DVZ::view_istream<char> istream{ sv };
		cereal::BinaryInputArchive archive{ istream };
		archive(packet);
		return true;
	}
}

#endif