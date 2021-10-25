#pragma once
#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include <thread>
#include <atomic>

namespace DVZ::Net {
	class ServerSocket;
}

namespace DVZ {
	class Engine {
	public:
		Engine();
		~Engine();

		void tick();
		void updateLoop();

	private:
		std::thread updateThread;
		std::atomic<bool> shouldRun;
	};
}

#endif