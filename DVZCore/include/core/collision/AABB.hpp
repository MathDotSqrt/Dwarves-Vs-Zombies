#pragma once
#ifndef DVZ_AABB_HPP
#define DVZ_AABB_HPP

#include <glm/glm.hpp>

namespace DVZ::Collision {
	struct AABB {
		AABB(const glm::vec3& min, const glm::vec3& max);

		glm::vec3 min;
		glm::vec3 max;

		glm::vec3 getPositive(const glm::vec3& normal) const;
		glm::vec3 getNegative(const glm::vec3& normal) const;
	};
}

#endif