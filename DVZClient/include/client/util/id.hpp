#pragma once

#ifndef DVZ_ID_HPP
#define DVZ_ID_HPP

#include "core/common.hpp"

namespace DVZ {
	struct ID {
		u32 index = 0;
		u32 generation = 0;

		bool operator==(const ID& other) const {
			return index == other.index && generation == other.generation;
		}
	};
	constexpr ID NullID = ID{ (u32)-1, (u32)-1 };
}

#endif