#include "client/net/InterpolateNetValues.hpp"

#include "spdlog/spdlog.h"

using namespace DVZ;
using namespace DVZ::Net;

//https://developer.valvesoftware.com/wiki/Latency_Compensating_Methods_in_Client/Server_In-game_Protocol_Design_and_Optimization

InterpolateNetValues::InterpolateNetValues() {

}

PositionNetValues* InterpolateNetValues::insertPositionNetValues(simulation_duration client_time) {
	auto iter = std::find_if(buffer.begin(), buffer.end(), [&](const PositionHistory& entry) {
		return client_time > entry.server_time;
	});

	if (iter == buffer.end()) {
		PositionHistory& value = buffer.emplace_back(PositionHistory{ glm::vec3{0}, glm::quat{1, 0, 0, 0} });
		value.server_time = client_time;
		return &value.values;
	}
	else {
		PositionHistory& value = *buffer.insert(iter, *iter);
		value.server_time = client_time;
		return &value.values;
	}


	//if (buffer.size() >= 3) {
	//	const auto erase_iter = std::remove_if(buffer.begin() + 2, buffer.end(), [&](const PositionHistory& entry) {
	//		return (server_time - entry.server_time) > (1 * interpolation_offset);
	//	});
	//	buffer.erase(erase_iter, buffer.end());
	//}
}

std::optional<PositionNetValues> InterpolateNetValues::computeInterpolation(simulation_duration client_time, const std::vector<simulation_duration>& ackBuffer) const {
	simulation_duration target_time = client_time - interpolation_offset;

	if (buffer.size() == 0) {
		spdlog::info("0 buffer");
		return {};
	}

	if (target_time >= buffer.front().server_time) {
		//spdlog::info("out paced");
		return buffer.front().values;
	}

	for (size_t i = 0; i < buffer.size() - 1; i++) {
		const auto& t1 = buffer[i];
		const auto& t2 = buffer[i + 1];
		if (t1.server_time >= target_time && target_time >= t2.server_time) {
			//if ((t1.server_time - t2.server_time) == simulation_duration{1}) {
			//	return t2.values;
			//}

			const auto iter = std::find_if(ackBuffer.rbegin(), ackBuffer.rend(), [&](const simulation_duration& lastAck) {
				return target_time >= lastAck;
			});

			const simulation_duration t1_time = *(iter - 1);
			const simulation_duration t2_time = *iter;

			if (t1_time < t1.server_time) {
				return t2.values;
			}


			const auto delta = t1_time - t2_time;


			const auto denom = delta;
			const auto num = target_time - (t1_time - delta);
			const auto alpha = (float)num.count() / (float)denom.count();
			
			if (alpha == 0) {
				return t2.values;
			}

			const auto interpolate_pos = glm::mix(t2.values.pos, t1.values.pos, alpha);
			const auto interpolate_quat = glm::slerp(t2.values.rot, t1.values.rot, alpha);
			return PositionNetValues{ interpolate_pos, interpolate_quat};
		}
	}

	//spdlog::error("Oh shit");
	return {};
}

PositionNetValues* InterpolateNetValues::getLastBufferedValues() {
	if (buffer.size() > 0) {
		return &(buffer[0].values);
	}
	return {};
}
