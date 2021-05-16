#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/GeometryBuilder.hpp"

#include "client/window.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


using namespace DVZ::Graphics;

BasicRenderer::BasicRenderer() {
	using namespace entt;

	meshCache.load<MeshLoader>("cube"_hs, gen_cube(1));
	texCache.load<TextureLoader>("uv"_hs, TEX::Builder().mipmapLinear(), "./res/textures/uv_grid.jpg");

	auto shader = ss.load("basic"_hs, "basic.vert", "basic.frag");

	const auto& window = DVZ::Window::getInstance();
	projection = glm::perspectiveFov<float>(80, window.getWidth(), window.getHeight(), .1f, 100000.0f);

	glEnable(GL_DEPTH_TEST);
}

void BasicRenderer::prerender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BasicRenderer::render(const InterpolatedScene& scene) {
	using namespace entt;
	static float t = 0;
	t -= .01f;
	glm::quat rot = glm::angleAxis(t, glm::normalize(glm::vec3(1, 1, 1)));


	prerender();

	auto shader = ss.get("basic"_hs);
	auto tex = texCache.handle("uv"_hs);
	shader->start();
	for (const auto& instance : scene.instances) {
		auto mesh = meshCache.handle(instance.meshID);
		glm::mat4 M = glm::identity<glm::mat4>();
		M = glm::translate(M, instance.pos);
		M = M * glm::toMat4(rot);

		shader->setUniformMat4("MP", projection * M);
		shader->setUniform1i("diffuse", 0);

		tex->bindActiveTexture(0);

		mesh->vao.bind();
		mesh->ebo.bind();
		glDrawElements(GL_TRIANGLES, mesh->ebo.getNumBytes() / sizeof(u32), GL_UNSIGNED_INT, 0);
		mesh->ebo.unbind();
		mesh->vao.unbind();
	}



	

	tex->unbind();


	shader->end();
}