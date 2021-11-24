#ifndef DVZ_SERVER_COMPONENTS_HPP
#define DVZ_SERVER_COMPONENTS_HPP

#include "core/common.hpp"
#include "core/time.hpp"
namespace DVZ {
	struct Debug {
		float timer;
	};

	struct NetPlayer {
		constexpr static int UPS = 30;
		constexpr static auto DELAY = std::chrono::duration_cast<simulation_duration>(duration{ 1.0f / UPS });
		simulation_duration timer = DELAY;
	};
}

#endif