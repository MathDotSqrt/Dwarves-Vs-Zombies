#include "server/Engine.hpp"
#include "server/ServerNetRecvSystem.hpp"
#include "server/ServerNetSendSystem.hpp"
#include "server/PhysicsSystem.hpp"
#include "server/ServerVoxelSystem.hpp"
#include "server/net/NetServerManager.hpp"
#include "server/net/ServerSocket.hpp"
#include "server/voxel/ServerChunkManager.hpp"
#include "core/time.hpp"

#include "core/util/Timer.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ;

Engine::Engine() : 
	shouldRun(true), 
	registry(std::make_unique<entt::registry>()),
	netManager(std::make_unique<DVZ::Net::NetServerManager>()),
	chunkManager(std::make_unique<DVZ::Voxel::ServerChunkManager>()),
	netRecvSystem(std::make_unique<DVZ::ServerNetRecvSystem>()),
	netSendSystem(std::make_unique<DVZ::ServerNetSendSystem>()),
	physicsSystem(std::make_unique<DVZ::PhysicsSystem>()),
	voxelSystem(std::make_unique<DVZ::ServerVoxelSystem>()) {

	updateThread = std::make_unique<std::thread>(&Engine::updateLoop, this);
}

Engine::~Engine() {
	shouldRun = false;
	updateThread->join();
}

void Engine::init() {
	netRecvSystem->init(*this);
	physicsSystem->init(*this);
	voxelSystem->init(*this);
	netSendSystem->init(*this);
}

void Engine::tick() {
	netRecvSystem->tick(*this);
	physicsSystem->tick(*this);
	voxelSystem->tick(*this);
	netSendSystem->tick(*this);
}

void Engine::updateLoop() {
	
	init();

	const auto start_time = std::chrono::steady_clock::now();
	auto last_time = start_time;
	duration accum{ 0 };
	while (shouldRun.load()) {

		std::chrono::time_point new_time = std::chrono::steady_clock::now();
		duration frame_time = new_time - last_time;
		last_time = new_time;

		accum += frame_time;

		while (accum >= DT) {
			RootTimer root{ "ServerUpdate" };
			//last_update = std::chrono::steady_clock::now();
			total_time += DT;
			tick();
			accum -= DT;

			root.enablePrint(std::chrono::milliseconds{10});
			if (shouldPrintPerf) {
				root.enablePrint();
				shouldPrintPerf = false;
			}
		}
	}
}

void Engine::enablePerfPrint() {
	shouldPrintPerf = true;
}

entt::registry& Engine::getRegistry(){
	return *registry;
}

const entt::registry& Engine::getRegistry() const {
	return *registry;
}

Net::NetServerManager& Engine::getNetManager() {
	return *netManager;
}

Voxel::ServerChunkManager& Engine::getChunkManager() {
	return *chunkManager;
}

DVZ::simulation_duration Engine::getTimeElapsed() const {
	return total_time;
}