#include "core/util/transform.hpp"

glm::quat DVZ::Util::remove_pitch(const glm::quat& q) {
	glm::vec3 eular = glm::eulerAngles(q);
	return glm::quat(glm::vec3(eular.z, eular.y, eular.z));
}

glm::mat4 DVZ::Util::to_transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) {
	glm::mat4 M = glm::identity<glm::mat4>();
	M = glm::translate(M, pos);
	M = M * glm::toMat4(rot);
	M = glm::scale(M, scale);
	return M;
}