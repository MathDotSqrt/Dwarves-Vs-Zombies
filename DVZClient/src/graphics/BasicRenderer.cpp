#include "client/graphics/BasicRenderer.hpp"


#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "client/graphics/GeometryBuilder.hpp"

using namespace DVZ::Graphics;

BasicRenderer::BasicRenderer() {
	using namespace entt;

	cache.load<MeshLoader>("cube"_hs, gen_cube(1));
	auto shader = ss.load("basic"_hs, "basic.vert", "basic.frag");
}

void BasicRenderer::prerender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BasicRenderer::render() {
	using namespace entt;

	prerender();

	auto mesh = cache.handle("cube"_hs);
	auto shader = ss.get("basic"_hs);

	shader->start();

	shader->setUniformMat4("MP", glm::identity<glm::mat4>());

	mesh->vao.bind();
	mesh->ebo.bind();
	glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
	mesh->ebo.unbind();
	mesh->vao.unbind();

	shader->end();
}