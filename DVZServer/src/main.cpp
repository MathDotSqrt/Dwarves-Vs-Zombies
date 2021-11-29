#include "server/Engine.hpp"
#include "server/net/ServerSocket.hpp"

#include "core/net/EntitySnapshotDelta.hpp"
#include "core/util/serialization.hpp"

#include <spdlog/spdlog.h>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
	spdlog::info("Initializing server...");
	
	{
		DVZ::Engine engine;

		while (true) {
			//std::cout << "[Enter Command]>" << std::flush;
			std::string line;
			std::getline(std::cin, line);
			if (line.size() == 0) {
				break;
			}
			else {
				spdlog::info("Command: {}", line);
			}
		}
		spdlog::info("Shutting down...");
	}

	spdlog::info("Done.");
}