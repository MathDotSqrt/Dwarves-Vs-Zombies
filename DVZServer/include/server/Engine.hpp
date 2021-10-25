#pragma once
#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include <thread>
#include <atomic>
#include <memory>


namespace DVZ {
	class NetSystem;

	class Engine {
	public:
		Engine();
		~Engine();

		void tick();
		void updateLoop();
	private:
		std::thread updateThread;
		std::atomic<bool> shouldRun;

		std::unique_ptr<NetSystem> netSystem;
	};
}

#endif