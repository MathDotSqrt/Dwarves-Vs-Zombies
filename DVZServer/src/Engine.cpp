#include "server/Engine.hpp"
#include "server/NetSystem.hpp"
#include "server/net/ServerSocket.hpp"


#include <spdlog/spdlog.h>

using namespace DVZ;

Engine::Engine() : 
	shouldRun(true), 
	netSystem(std::make_unique<DVZ::NetSystem>(*this)), 
	updateThread(&Engine::updateLoop, this) {


	
}

Engine::~Engine() {
	shouldRun = false;
	updateThread.join();
}

void Engine::tick() {
	netSystem->tick(*this);
}

void Engine::updateLoop() {
	using duration = std::chrono::duration<float>;

	auto last_time = std::chrono::steady_clock::now();

	while (shouldRun.load()) {
		auto current_time = std::chrono::steady_clock::now();
		auto delta = current_time - last_time;
		if (delta > duration{1}) {
			last_time = current_time;
			tick();
		}
	}
}