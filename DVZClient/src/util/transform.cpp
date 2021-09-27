#include "client/util/transform.hpp"
#include "client/window.hpp"
glm::mat4 DVZ::Util::to_transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) {
	glm::mat4 M = glm::identity<glm::mat4>();
	M = glm::translate(M, pos);
	M = M * glm::toMat4(rot);
	M = glm::scale(M, scale);
	return M;
}

DVZ::Camera DVZ::Util::create_default_camera() {
	DVZ::Camera camera;
	camera.fov = 80;

	const DVZ::Window& window = DVZ::Window::getInstance();
	camera.width = window.getWidth();
	camera.height = window.getHeight();
	camera.near = .1f;
	camera.far = 10000.0f;

	return camera;
}