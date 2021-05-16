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
		duration dt{ 1 / 5.0f };

		std::atomic<bool> shouldStop = false;
		std::atomic<std::chrono::time_point<std::chrono::steady_clock>> last_update;

		entt::registry registry;
		Graphics::Scene scene;
		Graphics::SceneManager sceneManager;
		Graphics::BasicRenderer renderer;
		std::thread updateThread;
	};
}

#endif