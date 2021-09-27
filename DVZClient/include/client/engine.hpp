#pragma once

#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include "client/systems/System.hpp"
#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/Scene.hpp"
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <entt/entt.hpp>

namespace DVZ{
	struct ClientPlayer {
		glm::vec3 pos = glm::vec3(0, 0, 0);
		glm::quat rot = glm::quat(1, 0, 0, 0);
		glm::vec2 last_mouse_pos = glm::vec2(0);
	};

	class Engine {
	public:
		using duration = std::chrono::duration<float>;

		constexpr static float TPS = 60.0f;

		Engine();
		~Engine();

		template<typename SYS>
		void addSystem() {
			systems.emplace_back(std::make_unique<SYS>());
			systems.back()->init(*this);
		}

		void update(duration total_time);
		void updateLoop();
		void render();
		void signalStop();

		entt::registry& getRegistry();
		Graphics::Scene& getScene();
	private:
		ClientPlayer player;
		duration dt{ 1 / TPS };

		std::atomic<bool> shouldStop = false;
		std::atomic<std::chrono::time_point<std::chrono::steady_clock>> last_update;

		entt::registry registry;
		Graphics::Scene scene;
		Graphics::SceneManager sceneManager;
		Graphics::RenderScene renderScene;
		Graphics::BasicRenderer renderer;
		std::thread updateThread;

		std::vector<std::unique_ptr<Systems::System>> systems;
	};
}

#endif