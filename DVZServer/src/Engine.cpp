#include "server/Engine.hpp"
#include "server/NetSystem.hpp"
#include "server/PhysicsSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/net/ServerSocket.hpp"


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

entt::registry& Engine::getRegistry(){
	return *registry;
}

Net::NetServerManager& Engine::getNetManager() {
	return *netManager;
}