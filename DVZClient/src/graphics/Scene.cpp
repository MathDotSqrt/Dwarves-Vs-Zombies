#include "client/graphics/Scene.hpp"
#include "client/window.hpp"

#include <iterator>
#include <spdlog/spdlog.h>
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
	return instances.push_back(Instance{meshID});
}

bool Scene::hasInstance(DVZ::ID instanceID) const {
	return instances.has(instanceID);
}

Instance& Scene::getInstance(DVZ::ID instanceID) {
	assert(hasInstance(instanceID));
	return instances[instanceID];
}

const Instance& Scene::getInstance(DVZ::ID instanceID) const {
	assert(hasInstance(instanceID));
	return instances[instanceID];
}

SceneManager::SceneManager() {

}

void SceneManager::computeInterpolate(float alpha) {
	std::lock_guard<std::mutex> g{m};
	interpolatedScene.instances.clear();
	
	for (u32 i = 0; i < prev.instances.size(); i++) {
		bool has_prev = prev.instances.has(i);
		bool has_current = current.instances.has(i);

		if (has_current && has_prev) {
			u32 prev_gen = prev.instances.get_generation(i);
			u32 curr_gen = current.instances.get_generation(i);

			if (curr_gen == prev_gen) {
				//linear interpolate
				const Instance& prev_instance = prev.instances[i];
				const Instance& current_instance = current.instances[i];

				Instance interpolate{ current_instance.meshID };
				interpolate.pos = glm::mix(prev_instance.pos, current_instance.pos, alpha);
				interpolatedScene.instances.push_back(interpolate);
			}
		}
		else if (has_current) {
			interpolatedScene.instances.push_back(current.instances[i]);
		}
	}
}

void SceneManager::bufferScene(const Scene& scene) {
	std::lock_guard<std::mutex> g{m};
	prev = current;
	current = scene;
}

const InterpolatedScene& SceneManager::getInterpolatedScene() const {
	return interpolatedScene;
}