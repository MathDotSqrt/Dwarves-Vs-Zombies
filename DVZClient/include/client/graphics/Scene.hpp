#pragma once

#ifndef DVZ_SCENE_HPP
#define  DVZ_SCENE_HPP

#include "client/graphics/Mesh.hpp"
#include "client/util/packedfreelist.hpp"
#include "client/util/freelist.hpp"
#include "core/util/Frustum.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/core/hashed_string.hpp>
#include <mutex>

#undef near
#undef far

namespace DVZ::Graphics {

	class BasicRenderer;
	class SceneManager;

	struct PerspectiveCamera {
		float fov;
		float width;
		float height;
		float near;
		float far;

		glm::vec3 pos = glm::vec3(0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);

		PerspectiveCamera();
		PerspectiveCamera(float fov, float width, float height, float near, float far);

		Frustum computeFrustum() const;
	};

	struct Instance {
		entt::id_type meshID;
		glm::vec3 pos = glm::vec3{0};
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec3 scale = glm::vec3(1);
	};

	struct InterpolatedInstance {
		entt::id_type meshID;
		glm::mat4 transform;

		InterpolatedInstance(entt::id_type meshID, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);
		InterpolatedInstance(const Instance& instance);
	};

	class Scene {
	public:
		friend BasicRenderer;
		friend SceneManager;

		Scene();

		ID addInstance(entt::id_type meshID);
		void removeInstance(DVZ::ID instanceID);

		bool hasInstance(DVZ::ID instanceID) const;
		Instance& getInstance(DVZ::ID meshID);
		const Instance& getInstance(DVZ::ID meshID) const;

		void setPlayerCamera(const PerspectiveCamera& camera);
		const PerspectiveCamera& getPlayerCamera() const;

		const Frustum& getFrustum() const;
	private:
		PerspectiveCamera playerCamera;
		DVZ::Util::freelist<Instance> instances;
		Frustum view;
	};

	class InterpolatedScene {
	public:
		PerspectiveCamera playerCamera;
		std::vector<InterpolatedInstance> instances;
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