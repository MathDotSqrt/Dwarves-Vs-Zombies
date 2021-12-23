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
		Plane(float a, float b, float c, float d);
		Plane(const glm::vec3& pos, const glm::vec3& norm);

		glm::vec3 normal{0,1,0};
		float distance = 0.0f;
	};

	float signedDistanceToPlane(const Plane& plane, const glm::vec3& pos);

	class Frustum {
	public:
		Frustum(const glm::mat4& M);
		void computeFrustum(const glm::mat4& M);
		bool intersects(const DVZ::Collision::AABB& aabb) const;
	private:
		enum FrustumSection {
			NEAR = 0,
			FAR,
			LEFT,
			RIGHT,
			TOP,
			BOTTOM
		};

		std::array<Plane, 6> planes;
	};

}

#endif