#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <iostream>

#include "client/engine.hpp"
#include "client/window.hpp"

void set_log_level() {
#ifdef DVZ_CLIENT_DEBUG
	spdlog::set_level(spdlog::level::info);
#elif DVZ_CLIENT_RELDEBINFO
	spdlog::set_level(spdlog::level::info);
#else
	spdlog::set_level(spdlog::level::info);
#endif // DVZ_CLIENT_DEBUG
}

int main(int argc, char* argv[]) {
	using namespace DVZ::Graphics;
	using namespace entt;

	{
		std::string ip = "127.0.0.1:50150";
		if (argc == 1) {
			std::cout << "Select which IP to connect to\n";
			std::cout << "1) 127.0.0.1:50150\n";
			std::cout << "2) 54.242.226.228:50150\n";
			std::cout << "Enter IP address: ";
			int selection = 1;
			std::cin >> selection;

			if (selection == 2)
				ip = "54.242.226.228:50150";

		}
		else if (argc == 2) {
			ip = argv[1];
		}

		set_log_level();
		spdlog::info("Init");

		DVZ::Window& window = DVZ::Window::createInstance(1280, 1024, "Level One Adventure");

		glewExperimental = GL_TRUE;
		GLenum glewErr = glewInit();
		if (glewErr != GLEW_OK) {
			spdlog::error("GLEW FAILED");
		}

		DVZ::Engine engine{ip};


		while (!window.shouldClose()) {
			window.swapBuffers();
			engine.render();
			
		}
	}

	DVZ::Window::destroyInstance();
}