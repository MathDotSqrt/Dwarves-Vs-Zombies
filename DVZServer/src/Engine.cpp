#include "server/Engine.hpp"

using namespace DVZ;

Engine::Engine() : shouldRun(true), updateThread(&Engine::updateLoop, this){
	
}

Engine::~Engine() {
	shouldRun = false;
	updateThread.join();
}

void Engine::tick() {
	
}

void Engine::updateLoop() {
	using duration = std::chrono::duration<float>;

	auto last_time = std::chrono::steady_clock::now();

	while (shouldRun.load()) {
		auto current_time = std::chrono::steady_clock::now();
		auto delta = current_time - last_time;
		if (delta > duration{1/60.0f}) {
			last_time = current_time;
			tick();
		}
	}
}