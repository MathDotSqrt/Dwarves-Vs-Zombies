#pragma once

#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/Scene.hpp"
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
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

		Engine();
		~Engine();

		void update(duration total_time);
		void updateLoop();
		void render();
		void signalStop();

	private:
		
		ClientPlayer player;
		duration dt{ 1 / 50.0f };

		std::atomic<bool> shouldStop = false;
		std::atomic<std::chrono::time_point<std::chrono::steady_clock>> last_update;

		entt::registry registry;
		Graphics::Scene scene;
		Graphics::SceneManager sceneManager;
		Graphics::RenderScene renderScene;
		Graphics::BasicRenderer renderer;
		std::thread updateThread;
	};
}

#endif