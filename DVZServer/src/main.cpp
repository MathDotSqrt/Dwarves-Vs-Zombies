
#include "server/net/ServerSocket.hpp"

#include <spdlog/spdlog.h>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

std::atomic<bool> should_run = true;

void server_loop() {

	DVZ::Net::ServerSocket server{50150};


	auto last_time = std::chrono::steady_clock::now();
	while (should_run.load()) {
		auto current_time = std::chrono::steady_clock::now();
		auto delta = current_time - last_time;
		if (delta > std::chrono::seconds(1)) {
			last_time = current_time;
			server.pollIncomingMessages();
		}
	
	}
}

int main() {
	spdlog::info("Initializing server...");

	std::thread t{server_loop};

	std::this_thread::sleep_for(std::chrono::seconds(1));
	while (true) {
		std::cout << "[Enter Command]>" << std::flush;
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
	should_run = false;
	t.join();
	spdlog::info("Done.");
}