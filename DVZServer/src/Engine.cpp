#include "server/Engine.hpp"
#include "server/NetSystem.hpp"
#include "server/PhysicsSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/net/ServerSocket.hpp"
#include "core/time.hpp"


#include <spdlog/spdlog.h>

using namespace DVZ;

Engine::Engine() : 
	shouldRun(true), 
	registry(std::make_unique<entt::registry>()),
	netSystem(std::make_unique<DVZ::NetSystem>(*this)),
	physicsSystem(std::make_unique<DVZ::PhysicsSystem>(*this)),
	netManager(std::make_unique<DVZ::Net::NetServerManager>()),
	updateThread(&Engine::updateLoop, this) {


	
}

Engine::~Engine() {
	shouldRun = false;
	updateThread.join();
}

void Engine::tick() {
	netSystem->tick(*this);
	physicsSystem->tick(*this);
}

void Engine::updateLoop() {
	
	constexpr duration dt{ 1.0f / TPS };

	const auto start_time = std::chrono::steady_clock::now();
	auto last_time = start_time;
	duration accum{ 0 };
	while (shouldRun.load()) {
		std::chrono::time_point new_time = std::chrono::steady_clock::now();
		duration frame_time = new_time - last_time;
		last_time = new_time;

		accum += frame_time;

		while (accum >= dt) {
			//last_update = std::chrono::steady_clock::now();
			total_time += dt;
			tick();
			accum -= dt;
		}
	}
}

entt::registry& Engine::getRegistry(){
	return *registry;
}

Net::NetServerManager& Engine::getNetManager() {
	return *netManager;
}

DVZ::duration Engine::getTimeElapsed() const {
	return total_time;
}