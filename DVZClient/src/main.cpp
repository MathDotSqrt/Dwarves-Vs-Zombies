#include <iostream>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include "core/test.hpp"
#include "client/window.hpp"
#include "client/graphics/Attrib.hpp"
#include "client/graphics/Geometry.hpp"
#include "client/graphics/ShaderSet.hpp"
#include "client/graphics/VAO.hpp"
#include "client/graphics/VBO.hpp"
#include "glsl/preamble.glsl"

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
		std::cout << "FATAL\n";
	}

	DVZ::Graphics::Geometry<Position2DAttrib> quad;
	quad.pushVertex(glm::vec2{ 1, 1 });
	quad.pushVertex(glm::vec2{ -1, 1 });
	quad.pushVertex(glm::vec2{ -1, -1 });
	quad.pushVertex(glm::vec2{ 1, -1 });
	quad.pushTriangle(0, 1, 2);
	quad.pushTriangle(0, 2, 3);

	DVZ::Graphics::VAO vao;
	DVZ::Graphics::VBO vbo{ VBO::BufferType::ARRAY_BUFFER };
	DVZ::Graphics::VBO ebo{ VBO::BufferType::ELEMENT_ARRAY_BUFFER };

	vao.bind();

	vbo.bind();
	vao.addVertexAttribPtr<Position2DAttrib>();
	vbo.bufferData(quad.getVerticies());
	vbo.unbind();

	ebo.bind();
	ebo.bufferData(quad.getIndices());
	ebo.unbind();

	vao.unbind();


	


	DVZ::Graphics::ShaderSet ss;
	auto shader = ss.load("test"_hs, "basic.vert", "basic.frag");

	while (!window.shouldClose()) {

		shader->start();

		vao.bind();
		ebo.bind();
		glDrawElements(GL_TRIANGLES, ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
		ebo.unbind();
		vao.unbind();

		shader->end();

		window.update();
	}

	DVZ::Window::destroyInstance();
}