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
}

#endif