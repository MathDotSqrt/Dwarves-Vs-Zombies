#pragma once

#ifndef DVZ_TRANSFORM_HPP
#define DVZ_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <client/ClientComponents.hpp>

namespace DVZ::Util {
	DVZ::Camera	create_default_camera();

}
#endif