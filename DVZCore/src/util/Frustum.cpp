#include "core/util/Frustum.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <algorithm>

using namespace DVZ;

Plane::Plane() : normal(glm::vec3(0, 1, 0)), distance(0.0f) {
	
}

Plane::Plane(const glm::vec3& pos, const glm::vec3& normal) 
	: normal(glm::normalize(normal)), 
	distance(glm::dot(glm::normalize(normal), pos)) {

}

float DVZ::signedDistanceToPlane(const Plane& plane, const glm::vec3& pos) {
	return glm::dot(plane.normal, pos) - plane.distance;
}

Frustum::Frustum(const glm::vec3& pos, const glm::quat& rot, float fov, float aspect, float near, float far) {
	computeFrustum(pos, rot, fov, aspect, near, far);
}

void Frustum::computeFrustum(const glm::vec3& pos, const glm::quat& rot, float fov, float aspect, float near, float far) {
	constexpr glm::vec3 FORWARD_DIR{ 0, 0, -1 };
	constexpr glm::vec3 UP_DIR{ 0, 1, 0 };
	constexpr glm::vec3 RIGHT_DIR{1, 0, 0};
	
	
	const glm::vec3 forward = glm::normalize(rot * FORWARD_DIR);
	const glm::vec3 right = glm::normalize(rot * RIGHT_DIR);
	const glm::vec3 up = glm::normalize(rot * UP_DIR);

	const float halfVSide = far * glm::tan(fov * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = far * forward;

	planes[RIGHT] = Plane{ pos, glm::cross(up, frontMultFar + right * halfHSide) };
	planes[LEFT] = Plane{pos, glm::cross(frontMultFar - right * halfHSide, up)};
	
	planes[TOP] = Plane{ pos, glm::cross(right, frontMultFar - up * halfVSide) };
	planes[BOTTOM] = Plane{pos, glm::cross(frontMultFar + up * halfVSide, right)};

	planes[NEAR] = Plane{ pos + near * forward, forward };
	planes[FAR] = Plane{ pos + frontMultFar, -forward};
}

bool Frustum::intersects(const DVZ::Collision::AABB& aabb) const {
	const glm::vec3 center = (aabb.max + aabb.min) * .5f;
	const glm::vec3 extents = (aabb.max - aabb.min) * .5f;

	const auto isOnOrForwardPlane = [&](const Plane& plane) {
		const float r = glm::dot(extents, glm::abs(plane.normal));
		return -r <= DVZ::signedDistanceToPlane(plane, center);
	};
	
	return std::all_of(planes.begin(), planes.end(), isOnOrForwardPlane);
}