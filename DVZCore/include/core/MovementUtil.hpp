#ifndef DVZ_MOVEMENT_UTIL_HPP
#define DVZ_MOVEMENT_UTIL_HPP

#include "core/CoreComponents.hpp"

namespace DVZ {
	glm::quat rotatePlayer(const glm::vec2& delta, const glm::quat& current_rot, const Direction& dir);
	glm::vec3 computePlayerVelocity(const MovementState& state, const glm::quat& rot, const Direction& dir);
}

#endif 