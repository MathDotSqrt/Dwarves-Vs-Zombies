#pragma once

#ifndef DVZ_COMPONENTS_HPP
#define DVZ_COMPONENTS_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include "client/util/packedfreelist.hpp"


namespace DVZ {
	struct Transformation {
		glm::vec3 pos = glm::vec3(0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);
	};

	struct Velocity : glm::vec3 {
		Velocity() : glm::vec3(0) {

		}
		
		Velocity(const glm::vec3& vel) : glm::vec3(vel) {
		
		}

		operator const glm::vec3& () const { 
			return static_cast<glm::vec3>(*this); 
		}
	};

	struct Renderable {
		entt::id_type mesh_id;
		DVZ::ID instance_id = DVZ::NullID;
	};

	struct MovementState {
		float forward = 0;
		float strafe = 0;
		float fly = 0;
	};

	struct Direction {
		glm::vec3 forward{ 0, 0, -1 };
		glm::vec3 up{ 0, 1, 0 };
		glm::vec3 right{1, 0, 0};
	};

	struct Player {
		//Player Tag
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