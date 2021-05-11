#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include "core/test.hpp"
#include "client/window.hpp"
#include "client/graphics/ShaderSet.hpp"

int main() {
	using namespace entt;

	spdlog::set_level(spdlog::level::debug);
	spdlog::info("Init");

	DVZ::Window& window = DVZ::Window::createInstance(1280, 1024, "Level One Adventure");
	std::cout << DVZ::add(10, 10);
	
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::cout << "FATAL\n";
	}

	DVZ::Graphics::ShaderSet ss;
	auto shader = ss.load("test"_hs, "basic.vert", "basic.frag");

	while (!window.shouldClose()) {
		window.update();
	}

	DVZ::Window::destroyInstance();
	exit(EXIT_SUCCESS);
}