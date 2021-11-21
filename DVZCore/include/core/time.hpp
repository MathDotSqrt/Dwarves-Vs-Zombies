#ifndef DVZ_TIME_HPP
#define DVZ_TIME_HPP
#include "core/common.hpp"
#include <chrono>

namespace DVZ {
	constexpr int TPS = 10;

	using duration = std::chrono::duration<float>;
	using simulation_duration = std::chrono::duration<u32, std::ratio<1, TPS>>;

	constexpr simulation_duration DT{1};
}

#endif 