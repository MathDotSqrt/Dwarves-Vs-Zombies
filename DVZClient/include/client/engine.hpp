#pragma once

#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/Scene.hpp"
#include <chrono>
#include <atomic>
#include <thread>

namespace DVZ{
	class Engine {
	public:
		Engine();
		~Engine();

		void updateLoop();
		void update(float delta);
		void signalStop();

	private:
		std::atomic<bool> shouldStop = false;
		Graphics::SceneManager sceneManager;
		Graphics::BasicRenderer renderer;
		std::thread updateThread;
	};
}

#endif