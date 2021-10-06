#pragma once

#ifndef DVZ_CLIENT_COMPONENTS_HPP
#define DVZ_CLIENT_COMPONENTS_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include "client/util/packedfreelist.hpp"


namespace DVZ {

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