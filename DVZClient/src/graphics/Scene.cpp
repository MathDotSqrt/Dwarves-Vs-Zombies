#include "client/graphics/Scene.hpp"
#include "client/window.hpp"


using namespace DVZ;
using namespace DVZ::Graphics;


PerspectiveCamera::PerspectiveCamera() {
	this->fov = 80;

	const DVZ::Window& window = DVZ::Window::getInstance();
	this->width = window.getWidth();
	this->height = window.getHeight();
	this->near = .1f;
	this->far = 10000.0f;
}

PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float near, float far) {
	this->fov = fov;
	this->width = width;
	this->height = height;
	this->near = near;
	this->far = far;
}

ID Scene::addInstance(entt::id_type meshID) {
	return addInstance(meshID, glm::identity<glm::mat4>());
}

ID Scene::addInstance(entt::id_type meshID, const glm::mat4& transform){
	return instances.insert(Instance{meshID, transform});
}

Instance& Scene::getInstance(entt::id_type meshID) {
	return instances[meshID];
}

const Instance& Scene::getInstance(entt::id_type meshID) const {
	return instances[meshID];
}