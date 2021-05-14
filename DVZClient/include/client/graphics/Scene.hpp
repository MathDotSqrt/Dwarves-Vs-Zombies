#pragma once

#ifndef DVZ_SCENE_HPP
#define  DVZ_SCENE_HPP

#include "client/graphics/Mesh.hpp"
#include "client/util/packedfreelist.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <entt/core/hashed_string.hpp>


namespace DVZ::Graphics {

	class BasicRenderer;

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
		glm::mat4 transform = glm::identity<glm::mat4>();
	};

	class Scene {
	public:
		friend BasicRenderer;

		ID addInstance(entt::id_type meshID);
		ID addInstance(entt::id_type meshID, const glm::mat4& transform);

		Instance& getInstance(entt::id_type meshID);
		const Instance& getInstance(entt::id_type meshID) const;
	private:
		DVZ::Util::PackedFreeList<Instance> instances;
	};

	class SceneManager {
		void swapScenes();
	private:
		Scene current;
		Scene prev;
	};

	class SceneCache {
		std::vector<Instance> instances;
	};
}

#endif