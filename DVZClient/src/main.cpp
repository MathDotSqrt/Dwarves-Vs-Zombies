#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "client/engine.hpp"
#include "client/window.hpp"

void set_log_level() {
#ifdef DVZ_CLIENT_DEBUG
	spdlog::set_level(spdlog::level::debug);
#elif DVZ_CLIENT_RELDEBINFO
	spdlog::set_level(spdlog::level::info);
#else
	spdlog::set_level(spdlog::level::err);
#endif // DVZ_CLIENT_DEBUG
}

int main() {
	using namespace DVZ::Graphics;
	using namespace entt;

	set_log_level();
	spdlog::info("Init");

	DVZ::Window& window = DVZ::Window::createInstance(1280, 1024, "Level One Adventure");
	
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		spdlog::error("GLEW FAILED");
	}

	DVZ::Engine engine;
	while (!window.shouldClose()) {
		engine.render();
		window.update();
	}
	engine.signalStop();

	DVZ::Window::destroyInstance();
}