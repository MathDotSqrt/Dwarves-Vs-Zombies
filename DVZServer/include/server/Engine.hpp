#pragma once
#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include "core/time.hpp"

#include <entt/entt.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>


namespace DVZ {
	namespace Net {
		class NetServerManager;
	}

	namespace Voxel {
		class ServerChunkManager;
	}

	class ServerNetRecvSystem;
	class ServerNetSendSystem;
	class PhysicsSystem;
	class ServerVoxelSystem;

	using duration = std::chrono::duration<float>;

	class Engine {
	public:
		Engine();
		~Engine();

		void init();
		void tick();
		void updateLoop();

		entt::registry& getRegistry();
		const entt::registry& getRegistry() const;
		Net::NetServerManager& getNetManager();
		Voxel::ServerChunkManager& getChunkManager();
		simulation_duration getTimeElapsed() const;

	private:
		std::thread updateThread;
		std::atomic<bool> shouldRun;

		std::unique_ptr<entt::registry> registry;

		std::unique_ptr<ServerNetRecvSystem> netRecvSystem;
		std::unique_ptr<ServerNetSendSystem> netSendSystem;
		std::unique_ptr<PhysicsSystem> physicsSystem;
		std::unique_ptr<ServerVoxelSystem> voxelSystem;

		std::unique_ptr<Net::NetServerManager> netManager;
		std::unique_ptr<Voxel::ServerChunkManager> chunkManager;
		simulation_duration total_time{0};
	};
}

#endif