#pragma once
#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include <entt/entt.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>


namespace DVZ {
	namespace Net {
		class NetServerManager;
	}

	
	class NetSystem;

	class Engine {
	public:
		Engine();
		~Engine();

		void tick();
		void updateLoop();

		Net::NetServerManager& getNetManager();

	private:
		std::thread updateThread;
		std::atomic<bool> shouldRun;
		std::unique_ptr<NetSystem> netSystem;
		std::unique_ptr<Net::NetServerManager> netManager;
	};
}

#endif