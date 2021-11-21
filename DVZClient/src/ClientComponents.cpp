#include "client/ClientComponents.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace DVZ;

InterpolateNetValues::InterpolateNetValues() {
	
}

InterpolateNetValues::InterpolateNetValues(glm::vec3 pos, glm::quat rot, duration server_time) {
	buffer.emplace_back(PositionHistory{ pos, rot, server_time });
}

std::optional<PositionHistory> InterpolateNetValues::computeInterpolation(duration target_time) const{
	if (buffer.size() == 0) {
		return {};
	}
	
	if (buffer.size() == 1) {
		if (target_time >= buffer[0].server_time) {
			return getMostRecentHistory();
		}
		else { 
			return getMostRecentHistory();
		}
	}

	for (size_t i = 0; i < buffer.size() - 1; i++) {
		const auto& t1 = buffer[i];
		const auto& t2 = buffer[i + 1];

		if (t1.server_time >= target_time && target_time > t2.server_time) {
			const auto denom = t1.server_time - t2.server_time;
			const auto num = target_time - t2.server_time;
			const auto alpha = num / denom;
			if (alpha > 1) {
				spdlog::info("INFO: {}", alpha);
			}
			const auto interpolate_pos = glm::mix(t1.pos, t2.pos, alpha);
			const auto interpolate_quat = glm::slerp(t1.rot, t2.rot, alpha);
			return PositionHistory{interpolate_pos, interpolate_quat, target_time};
		}
	}
	spdlog::info("SNIP");
	return {};
}

void InterpolateNetValues::appendHistory(const PositionHistory& new_entry, duration client_time) {
	const auto iter = std::find_if(buffer.begin(), buffer.end(), [&](const PositionHistory& entry) {
		return new_entry.server_time > entry.server_time;
	});

	buffer.insert(iter, new_entry);

	//const auto erase_iter = std::remove_if(buffer.begin(), buffer.end(), [&](const PositionHistory& entry) {
	//	return (client_time - entry.server_time) > std::chrono::milliseconds(100);
	//});
	//buffer.erase(erase_iter, buffer.end());
}

const PositionHistory& InterpolateNetValues::getMostRecentHistory() const {
	return buffer[0];
}