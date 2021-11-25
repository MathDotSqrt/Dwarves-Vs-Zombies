#include "client/util/camera_util.hpp"
#include "client/window.hpp"

DVZ::Camera DVZ::Util::create_default_camera() {
	DVZ::Camera camera;
	camera.fov = 80;

	const DVZ::Window& window = DVZ::Window::getInstance();
	camera.width = (float)window.getWidth();
	camera.height = (float)window.getHeight();
	camera.near = .1f;
	camera.far = 10000.0f;

	return camera;
}