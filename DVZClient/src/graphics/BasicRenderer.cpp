#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/GeometryBuilder.hpp"

#include "client/window.hpp"
#include "client/util/transform.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <spdlog/spdlog.h>

using namespace DVZ::Graphics;

BasicRenderer::BasicRenderer() {
	using namespace entt;

	meshCache.load<MeshLoader>("cube"_hs, gen_cube(1));
	texCache.load<TextureLoader>("uv"_hs, TEX::Builder().mipmapLinear(), "./res/textures/uv_grid.jpg");

	auto shader = ss.load("basic"_hs, "basic.vert", "basic.frag");
		
	const auto& window = DVZ::Window::getInstance();
	P = glm::perspectiveFov<float>(80, window.getWidth(), window.getHeight(), .1f, 100000.0f);
	V = glm::identity<glm::mat4>();

	glEnable(GL_DEPTH_TEST);
}

void BasicRenderer::prerender(const PerspectiveCamera& camera) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	const auto& window = DVZ::Window::getInstance();
	P = glm::perspectiveFov<float>(camera.fov, window.getWidth(), window.getHeight(), camera.near, camera.far);

	V = glm::inverse(Util::to_transform(-camera.pos, camera.rot));
}

void BasicRenderer::render(const InterpolatedScene& scene) {
	using namespace entt;

	prerender(scene.playerCamera);

	auto shader = ss.get("basic"_hs);
	auto tex = texCache.handle("uv"_hs);

	shader->start();
	for (const auto& instance : scene.instances) {
		auto mesh = meshCache.handle(instance.meshID);
		glm::mat4 M = instance.transform;

		shader->setUniformMat4("MVP", P * V * M);
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