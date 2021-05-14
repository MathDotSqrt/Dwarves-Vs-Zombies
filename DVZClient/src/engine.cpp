#include "client/engine.hpp"
#include "client/graphics/GeometryBuilder.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
using namespace DVZ;

Engine::Engine() : updateThread(&Engine::updateLoop, this){
	using namespace entt;

}

Engine::~Engine() {
	signalStop();

	updateThread.join();
}

void Engine::updateLoop() {
	using duration = std::chrono::duration<float>;

	duration t{ 0.0 };
	duration dt{ 1 / 10.0 };

	spdlog::info("Engine: launched update loop with ticks per second: [{}]", 1 / dt.count());

	std::chrono::time_point current_time = std::chrono::system_clock::now();
	duration accum{0};
	while (!shouldStop) {
		std::chrono::time_point new_time = std::chrono::system_clock::now();
		duration frame_time = new_time - current_time;
		current_time = new_time;

		accum += frame_time;

		while (accum >= dt) {
			//lock scene manager
			spdlog::debug("Update");
			t += dt;
			accum -= dt;
		}

		float alpha = accum / dt;
	}
}

void Engine::update(float delta) {
	//renderer.render();
}

void Engine::signalStop() {
	shouldStop = true;
}