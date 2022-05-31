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
	
	struct Renderable {
		entt::id_type mesh_id;
		DVZ::ID instance_id = DVZ::NullID;
	};

	struct Input {
		glm::vec2 current_cursor_pos = glm::vec2{ 0 };
		glm::vec2 last_cursor_pos = glm::vec2{ 0 };
		bool left_click = false;
		bool right_click = false;
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