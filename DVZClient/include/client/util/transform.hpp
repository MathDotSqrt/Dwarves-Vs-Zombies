#pragma once

#ifndef DVZ_TRANSFORM_HPP
#define DVZ_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <client/ClientComponents.hpp>

namespace DVZ::Util {
	glm::quat remove_pitch(const glm::quat& q);
	glm::mat4 to_transform(const glm::vec3& pos = glm::vec3(0), const glm::quat& rot = glm::quat(1, 0, 0, 0), const glm::vec3& scale = glm::vec3(1));
	DVZ::Camera	create_default_camera();

}
#endif