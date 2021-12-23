#include "core/collision/AABB.hpp"

using namespace DVZ::Collision;

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max){

}

glm::vec3 AABB::getPositive(const glm::vec3& normal) const {
	glm::vec3 result = min;
	glm::vec3 delta = max - min;

	if (normal.x > 0)
		result.x += delta.x;
	if (normal.y > 0)
		result.y += delta.y;
	if (normal.z > 0)
		result.z += delta.z;

	return result;
}

glm::vec3 AABB::getNegative(const glm::vec3& normal) const {
	glm::vec3 result = min;
	glm::vec3 delta = max - min;

	if (normal.x < 0)
		result.x += delta.x;
	if (normal.y < 0)
		result.y += delta.y;
	if (normal.z < 0)
		result.z += delta.z;

	return result;
}
