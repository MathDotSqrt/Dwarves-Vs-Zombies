#pragma once

#ifndef DVZ_SCENE_HPP
#define  DVZ_SCENE_HPP

#include "client/graphics/Mesh.hpp"
#include "client/util/packedfreelist.hpp"
#include "client/util/freelist.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include <mutex>


namespace DVZ::Graphics {

	class BasicRenderer;
	class SceneManager;

	struct PerspectiveCamera {
		float fov;
		float width;
		float height;
		float near;
		float far;

		glm::mat4 transform = glm::identity<glm::mat4>();

		PerspectiveCamera();
		PerspectiveCamera(float fov, float width, float height, float near, float far);
	};

	struct Instance {
		entt::id_type meshID;
		glm::vec3 pos = glm::vec3{0};
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);
	};

	class Scene {
	public:
		friend BasicRenderer;
		friend SceneManager;

		ID addInstance(entt::id_type meshID);

		bool hasInstance(DVZ::ID instanceID) const;
		Instance& getInstance(DVZ::ID meshID);
		const Instance& getInstance(DVZ::ID meshID) const;
	private:
		DVZ::Util::freelist<Instance> instances;
	};

	class InterpolatedScene {
	public:
		std::vector<Instance> instances;
	};

	class SceneManager {
	public:
		SceneManager();

		void bufferScene(const Scene& scene);
		void computeInterpolate(float alpha);

		const InterpolatedScene& getInterpolatedScene() const;
	private:
		std::mutex m;
		Scene current;
		Scene prev;

		InterpolatedScene interpolatedScene;
	};
	
}

#endif