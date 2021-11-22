#include "client/ClientComponents.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace DVZ;

InterpolateNetValues::InterpolateNetValues() {
	
}

InterpolateNetValues::InterpolateNetValues(glm::vec3 pos, glm::quat rot, simulation_duration server_time) {
	buffer.emplace_back(PositionHistory{ pos, rot, server_time });
}

std::optional<PositionHistory> InterpolateNetValues::computeInterpolation(simulation_duration client_time) const{
	simulation_duration target_time = client_time - interpolation_offset;
	
	if (buffer.size() == 0) {
		return {};
	}
	
	if (target_time > buffer[0].server_time) {
		return buffer[0];
	}

	for (size_t i = 0; i < buffer.size() - 1; i++) {
		const auto& t1 = buffer[i];
		const auto& t2 = buffer[i + 1];

		if (t1.server_time >= target_time && target_time >= t2.server_time) {
			const auto denom = t1.server_time - t2.server_time;
			const auto num = target_time - t2.server_time;
			const auto alpha = (float)num.count() / (float)denom.count();

			const auto interpolate_pos = glm::mix(t2.pos, t1.pos, alpha);
			const auto interpolate_quat = glm::slerp(t2.rot, t1.rot, alpha);
			return PositionHistory{interpolate_pos, interpolate_quat, target_time};
		}
	}

	spdlog::error("Should not be here");
	return {};
}

void InterpolateNetValues::appendHistory(const PositionHistory& new_entry, simulation_duration client_time) {
	const auto iter = std::find_if(buffer.begin(), buffer.end(), [&](const PositionHistory& entry) {
		return new_entry.server_time >= entry.server_time;
	});

	buffer.insert(iter, new_entry);

	if (buffer.size() >= 3) {
		const auto erase_iter = std::remove_if(buffer.begin() + 2, buffer.end(), [&](const PositionHistory& entry) {
			return (client_time - entry.server_time) > (2 * interpolation_offset);
		});
		buffer.erase(erase_iter, buffer.end());
	}
}

const PositionHistory& InterpolateNetValues::getMostRecentHistory() const {
	return buffer[0];
}