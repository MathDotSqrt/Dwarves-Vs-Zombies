#include "client/net/InterpolateNetValues.hpp"

#include "spdlog/spdlog.h"

using namespace DVZ;
using namespace DVZ::Net;

//https://developer.valvesoftware.com/wiki/Latency_Compensating_Methods_in_Client/Server_In-game_Protocol_Design_and_Optimization

InterpolateNetValues::InterpolateNetValues() {

}

void InterpolateNetValues::appendHistory(const PositionNetValues& values, simulation_duration client_time) {

	PositionHistory new_entry{ values, client_time };

	const auto iter = std::find_if(buffer.begin(), buffer.end(), [&](const PositionHistory& entry) {
		return new_entry.server_time > entry.server_time;
	});

	buffer.insert(iter, new_entry);

	//if (buffer.size() >= 3) {
	//	const auto erase_iter = std::remove_if(buffer.begin() + 2, buffer.end(), [&](const PositionHistory& entry) {
	//		return (server_time - entry.server_time) > (1 * interpolation_offset);
	//	});
	//	buffer.erase(erase_iter, buffer.end());
	//}
}

std::optional<PositionNetValues> InterpolateNetValues::computeInterpolation(simulation_duration client_time) const {
	simulation_duration target_time = client_time - interpolation_offset;

	if (buffer.size() == 0) {
		spdlog::info("0 buffer");
		return {};
	}

	if (target_time > buffer.front().server_time) {
		//spdlog::info("out paced");
		return buffer.front().values;
	}

	for (size_t i = 0; i < buffer.size() - 1; i++) {
		const auto& t1 = buffer[i];
		const auto& t2 = buffer[i + 1];
		if (t1.server_time >= target_time && target_time >= t2.server_time) {
			//const auto delta = std::min(t1.server_time - t2.server_time, interpolation_offset);
			const auto delta = t1.server_time - t2.server_time;

			const auto denom = delta;
			const auto num = target_time - (t1.server_time - delta);
			const auto alpha = 0.0f;//(float)num.count() / (float)denom.count();

			const auto interpolate_pos = glm::mix(t2.values.pos, t1.values.pos, alpha);
			const auto interpolate_quat = glm::slerp(t2.values.rot, t1.values.rot, alpha);
			return PositionNetValues{ interpolate_pos, interpolate_quat};
		}
	}

	spdlog::error("Oh shit");
	return buffer.back().values;
}

std::optional<PositionNetValues> InterpolateNetValues::getLastBufferedValues() const {
	if (buffer.size() > 0) {
		return buffer[0].values;
	}
	return {};
}
