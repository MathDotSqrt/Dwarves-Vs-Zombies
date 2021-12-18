#include "core/util/Frustum.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <algorithm>

using namespace DVZ;

Plane DVZ::normalizePlane(const Plane& plane) {
	const auto mag = glm::length(glm::vec3(plane));
	return plane / mag;
}

HalfSpace DVZ::classifyPoint(const Plane& plane, const glm::vec3& point) {
	const auto d = glm::dot(glm::vec3(plane), point) + plane.w;
	if (d < 0) return HalfSpace::NEGATIVE;
	if (d > 0) return HalfSpace::POSITIVE;
	return HalfSpace::ON_PLANE;
}

Frustum::Frustum(const glm::mat4& M) {
	computeFrustum(M);
}

void Frustum::computeFrustum(const glm::mat4& M) {
	auto& left = planes[LEFT];
	left.x = M[3][0] + M[0][0];
	left.y = M[3][1] + M[0][1];
	left.z = M[3][2] + M[0][2];
	left.w = M[3][3] + M[0][3];

	auto& right = planes[RIGHT];
	right.x = M[3][0] - M[0][0];
	right.y = M[3][1] - M[0][1];
	right.z = M[3][2] - M[0][2];
	right.w = M[3][3] - M[0][3];

	auto& top = planes[TOP];
	top.x = M[3][0] - M[1][0];
	top.y = M[3][1] - M[1][1];
	top.z = M[3][2] - M[1][2];
	top.w = M[3][3] - M[1][3];

	auto& bottom = planes[BOTTOM];
	bottom.x = M[3][0] + M[1][0];
	bottom.y = M[3][1] + M[1][1];
	bottom.z = M[3][2] + M[1][2];
	bottom.w = M[3][3] + M[1][3];

	auto& near = planes[NEAR];
	near.x = M[3][0] + M[2][0];
	near.y = M[3][1] + M[2][1];
	near.z = M[3][2] + M[2][2];
	near.w = M[3][3] + M[2][3];

	auto& far = planes[FAR];
	far.x = M[3][0] - M[2][0];
	far.y = M[3][1] - M[2][1];
	far.z = M[3][2] - M[2][2];
	far.w = M[3][3] - M[2][3];

	left = normalizePlane(left);
	right = normalizePlane(right);
	top = normalizePlane(top);
	bottom = normalizePlane(bottom);
	near = normalizePlane(near);
	far = normalizePlane(far);
}

bool Frustum::intersects(const DVZ::Collision::AABB& aabb) const {
	std::array<glm::vec3, 8> points;
	points[0] = glm::vec3{aabb.min.x, aabb.min.y, aabb.min.z};
	points[1] = glm::vec3{ aabb.min.x, aabb.min.y, aabb.max.z };
	points[2] = glm::vec3{ aabb.min.x, aabb.max.y, aabb.min.z };
	points[3] = glm::vec3{ aabb.min.x, aabb.max.y, aabb.max.z };

	points[4] = glm::vec3{ aabb.max.x, aabb.min.y, aabb.min.z };
	points[5] = glm::vec3{ aabb.max.x, aabb.min.y, aabb.max.z };
	points[6] = glm::vec3{ aabb.max.x, aabb.max.y, aabb.min.z };
	points[7] = glm::vec3{ aabb.max.x, aabb.max.y, aabb.max.z };


	
	const auto is_outside = [&](const Plane& plane) {
		const auto is_negative = [&](const glm::vec3& point) {
			return classifyPoint(plane, point) == HalfSpace::NEGATIVE;
		};

		return std::all_of(points.begin(), points.end(), is_negative);
	};
	return !std::any_of(planes.begin(), planes.end(), is_outside);
}