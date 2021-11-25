#include "core/MovementUtil.hpp"
#include "core/util/transform.hpp"

using namespace DVZ;

glm::quat DVZ::rotatePlayer(const glm::vec2& delta, const glm::quat& current_rot, const Direction& dir) {
	glm::quat q_yaw = glm::angleAxis(-delta.x / 110, glm::vec3(0, 1, 0));
	glm::quat q_pitch = glm::angleAxis(-delta.y / 140, glm::vec3(1, 0, 0));

	glm::quat new_rot = (q_yaw * current_rot) * q_pitch;
	glm::quat move_dir = Util::remove_pitch(new_rot);

	if (glm::dot(new_rot * dir.forward, move_dir * dir.forward) < .01f) {
		return q_yaw * current_rot;
	}
	else {
		return new_rot;
	}
}

glm::vec3 DVZ::computePlayerVelocity(const MovementState& state, const glm::quat& rot, const Direction& dir) {
	glm::quat move_dir = Util::remove_pitch(rot);

	glm::vec3 forward_vel = state.forward * (move_dir * dir.forward);
	glm::vec3 strafe_vel = state.strafe * (move_dir * dir.right);
	glm::vec3 fly_vel = state.fly * dir.up;

	glm::vec3 vel = .4f * (forward_vel + strafe_vel + fly_vel);
	return vel;
}