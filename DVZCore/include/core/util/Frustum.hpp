#ifndef DVZ_FRUSTUM_HPP
#define DVZ_FRUSTUM_HPP

#include "core/collision/AABB.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

namespace DVZ {
	enum class HalfSpace {
		NEGATIVE = -1,
		ON_PLANE = 0,
		POSITIVE = 1
	};

	struct Plane {
		Plane();
		Plane(const glm::vec3& pos, const glm::vec3& norm);

		glm::vec3 normal{0,1,0};
		float distance = 0.0f;
	};

	float signedDistanceToPlane(const Plane& plane, const glm::vec3& pos);

	class Frustum {
	public:
		Frustum(const glm::vec3& pos, const glm::quat& rot, float fov, float aspec, float near, float far);
		void computeFrustum(const glm::vec3& pos, const glm::quat& rot, float fov, float aspec, float near, float far);
		bool intersects(const DVZ::Collision::AABB& aabb) const;
	private:

		std::array<Plane, 6> planes;
		constexpr static size_t LEFT = 0;
		constexpr static size_t RIGHT = 1;
		constexpr static size_t TOP = 2;
		constexpr static size_t BOTTOM = 3;
		constexpr static size_t NEAR = 4;
		constexpr static size_t FAR = 5;
	};

}

#endif