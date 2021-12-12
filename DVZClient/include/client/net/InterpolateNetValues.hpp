#ifndef DVZ_INTERPOLATE_NET_VALUES_HPP
#define DVZ_INTERPOLATE_NET_VALUES_HPP

#include "core/time.hpp"

#include <vector>
#include <chrono>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


namespace DVZ::Net {
	
	struct PositionNetValues {
		glm::vec3 pos;
		glm::quat rot;
	};

	struct InterpolateNetValues {
		constexpr static auto interpolation_offset = std::chrono::duration_cast<simulation_duration>(std::chrono::milliseconds(200));

		struct PositionHistory {
			PositionNetValues values;
			simulation_duration server_time;	//TODO: rename this variable
		};

		std::vector<PositionHistory> buffer;

		InterpolateNetValues();

		PositionNetValues* insertPositionNetValues(simulation_duration client_time);
		std::optional<PositionNetValues> computeInterpolation(simulation_duration client_time, const std::vector<simulation_duration>& ackBuffer) const;
		PositionNetValues* getLastBufferedValues();
	};
}

#endif
