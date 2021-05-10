#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "core/test.hpp"
#include "client/window.hpp"


int main() {
	DVZ::Window& window = DVZ::Window::createInstance(1280, 1024, "Level One Adventure");
	std::cout << DVZ::add(10, 10);
	
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::cout << "FATAL\n";
	}

	while (!window.shouldClose()) {
		window.update();
	}

	DVZ::Window::destroyInstance();
	exit(EXIT_SUCCESS);
}