#ifndef DVZ_FRUSTUM_HPP
#define DVZ_FRUSTUM_HPP

#include "core/collision/AABB.hpp"
#include <glm/glm.hpp>
#include <array>

namespace DVZ {
	enum class HalfSpace {
		NEGATIVE = -1,
		ON_PLANE = 0,
		POSITIVE = 1
	};

	using Plane = glm::vec4;

	Plane normalizePlane(const Plane& plane);
	HalfSpace classifyPoint(const Plane& plane, const glm::vec3& pos);

	class Frustum {
	public:
		Frustum(const glm::mat4& M);
		void computeFrustum(const glm::mat4& M);
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