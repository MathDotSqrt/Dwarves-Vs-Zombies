#include "core/util/Frustum.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <algorithm>

using namespace DVZ;

Plane::Plane() : normal(glm::vec3(0, 1, 0)), distance(0.0f) {
	
}

Plane::Plane(float a, float b, float c, float d) {
	const glm::vec3 vec{a, b, c};
	float mag = glm::length(vec);

	//TODO: check if its supposed to be vec / mag
	normal = glm::normalize(vec);
	distance = d / mag;
}

Plane::Plane(const glm::vec3& pos, const glm::vec3& normal) 
	: normal(glm::normalize(normal)), 
	distance(glm::dot(glm::normalize(normal), pos)) {

}

float DVZ::signedDistanceToPlane(const Plane& plane, const glm::vec3& pos) {
	return glm::dot(plane.normal, pos) - plane.distance;
}

Frustum::Frustum(const glm::mat4& M) {
	computeFrustum(M);
}

void Frustum::computeFrustum(const glm::mat4& M) {
	planes[TOP] = { M[0][3] - M[0][1], M[1][3] - M[1][1], M[2][3] - M[2][1], -M[3][3] + M[3][1] };
	planes[BOTTOM] = { M[0][3] + M[0][1], M[1][3] + M[1][1], M[2][3] + M[2][1], -M[3][3] - M[3][1] };
	planes[LEFT] = { M[0][3] + M[0][0], M[1][3] + M[1][0], M[2][3] + M[2][0], -M[3][3] - M[3][0] };
	planes[RIGHT] = { M[0][3] - M[0][0], M[1][3] - M[1][0], M[2][3] - M[2][0], -M[3][3] + M[3][0] };
	planes[NEAR] = { M[0][3] + M[0][2], M[1][3] + M[1][2], M[2][3] + M[2][2], -M[3][3] - M[3][2] };
	planes[FAR] = { M[0][3] - M[0][2], M[1][3] - M[1][2], M[2][3] - M[2][2], -M[3][3] + M[3][2] };
}

bool Frustum::intersects(const DVZ::Collision::AABB& aabb) const {
	for (size_t i = 0; i < 6; i++) {
		if (signedDistanceToPlane(planes[i], aabb.getPositive(planes[i].normal)) < 0) {
			return false;
		}
	}


	return true;
}