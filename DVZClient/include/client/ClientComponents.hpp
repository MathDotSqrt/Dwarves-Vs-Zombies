#pragma once

#ifndef DVZ_CLIENT_COMPONENTS_HPP
#define DVZ_CLIENT_COMPONENTS_HPP
#include "client/util/packedfreelist.hpp"

#include "core/time.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include <optional>
namespace DVZ {
	struct PositionHistory {
		glm::vec3 pos;
		glm::quat rot;
		simulation_duration server_time;
	};

	struct InterpolateNetValues {
		constexpr static auto interpolation_offset = std::chrono::duration_cast<simulation_duration>(std::chrono::milliseconds(100));
		std::vector<PositionHistory> buffer;

		InterpolateNetValues();
		InterpolateNetValues(glm::vec3, glm::quat, simulation_duration server_time);

		std::optional<PositionHistory> computeInterpolation(simulation_duration client_time) const;
		void appendHistory(const PositionHistory& entry, simulation_duration client_time);
		const PositionHistory& getMostRecentHistory() const;
	};

	struct Renderable {
		entt::id_type mesh_id;
		DVZ::ID instance_id = DVZ::NullID;
	};

	struct Input {
		glm::vec2 current_cursor_pos = glm::vec2{ 0 };
		glm::vec2 last_cursor_pos = glm::vec2{ 0 };
	};

	struct Camera {
		float fov;
		float width;
		float height;
		float near;
		float far;
	};
	
}

#endif