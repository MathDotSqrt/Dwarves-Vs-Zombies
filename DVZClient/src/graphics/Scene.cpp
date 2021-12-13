#include "client/graphics/Scene.hpp"
#include "client/window.hpp"
#include "core/util/transform.hpp"

#include <iterator>
#include <spdlog/spdlog.h>
using namespace DVZ;
using namespace DVZ::Graphics;

#undef near
#undef far

PerspectiveCamera::PerspectiveCamera() {
	this->fov = 80;

	const DVZ::Window& window = DVZ::Window::getInstance();
	this->width = (float)window.getWidth();
	this->height = (float)window.getHeight();
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

InterpolatedInstance::InterpolatedInstance(entt::id_type meshID, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) {
	this->meshID = meshID;
	this->transform = DVZ::Util::to_transform(pos, rot, scale);
}

InterpolatedInstance::InterpolatedInstance(const Instance& instance) 
	: InterpolatedInstance(instance.meshID, instance.pos, instance.rot, instance.scale){

}

ID Scene::addInstance(entt::id_type meshID) {
	return instances.push_back(Instance{meshID});
}

void Scene::removeInstance(DVZ::ID instanceID){
	instances.remove(instanceID);
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

Graphics::PerspectiveCamera& Scene::getPlayerCamera() {
	return playerCamera;
}

const Graphics::PerspectiveCamera& Scene::getPlayerCamera() const {
	return playerCamera;
}

SceneManager::SceneManager() {

}

void SceneManager::computeInterpolate(float alpha) {
	std::lock_guard<std::mutex> g{m};
	interpolatedScene.instances.clear();
	//spdlog::debug("{}", alpha);

	
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

				glm::vec3 pos = glm::mix(prev_instance.pos, current_instance.pos, alpha);
				glm::quat rot = glm::slerp(prev_instance.rot, current_instance.rot, alpha);
				glm::vec3 scale = glm::mix(prev_instance.scale, current_instance.scale, alpha);
				interpolatedScene.instances.emplace_back(current_instance.meshID, pos, rot, scale);
			}
			else {
				interpolatedScene.instances.push_back(current.instances[i]);
			}
		}
		else if (has_current) {
			interpolatedScene.instances.push_back(current.instances[i]);
		}
	}

	{
		const PerspectiveCamera& prev_camera = prev.getPlayerCamera();
		const PerspectiveCamera& current_camera = current.getPlayerCamera();
		float fov = glm::mix(prev_camera.fov, current_camera.fov, alpha);
		glm::vec3 pos = glm::mix(prev_camera.pos, current_camera.pos, alpha);
		glm::quat rot = glm::slerp(prev_camera.rot, current_camera.rot, alpha);
		glm::vec3 scale = glm::mix(prev_camera.scale, current_camera.scale, alpha);

		interpolatedScene.playerCamera.fov = fov;
		interpolatedScene.playerCamera.pos = pos;
		interpolatedScene.playerCamera.rot = rot;
		interpolatedScene.playerCamera.scale = scale;
		interpolatedScene.playerCamera.near = current_camera.near;
		interpolatedScene.playerCamera.far = current_camera.far;
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