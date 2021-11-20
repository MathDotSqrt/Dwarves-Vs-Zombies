#pragma once
#ifndef DVZ_CORE_COMPONENTS_HPP
#define DVZ_CORE_COMPONENTS_HPP

#include "core/collision/AABB.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

		//operator glm::vec3& () {
		//	return static_cast<glm::vec3&>(*this);
		//}
	};

	struct MovementState {
		float forward = 0;
		float strafe = 0;
		float fly = 0;
	};

	struct Direction {
		glm::vec3 forward{ 0, 0, -1 };
		glm::vec3 up{ 0, 1, 0 };
		glm::vec3 right{ 1, 0, 0 };
	};

	struct Player {
		//Player Tag
	};

	struct VoxelCollider {
		Collision::AABB collider;
	};

	struct Network {
		glm::vec3 last_pos = glm::vec3{0};
		glm::quat last_rot = glm::quat(1, 0, 0, 0);
		float duration_seconds = 0;
	};
}

#endif