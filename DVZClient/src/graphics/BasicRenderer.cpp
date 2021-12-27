#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/GeometryBuilder.hpp"
#include "client/graphics/Attrib.hpp"
#include "client/voxel/ChunkRenderDataManager.hpp"
#include "client/window.hpp"
#include "core/util/transform.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <spdlog/spdlog.h>

#undef near
#undef far

using namespace DVZ::Graphics;

BasicRenderer::BasicRenderer() {
	using namespace entt;

	meshCache.load<MeshLoader>("cube"_hs, gen_cube(1));
	texCache.load<TextureLoader>("uv"_hs, TEX::Builder().mipmapLinear(), "./res/textures/uv_grid.jpg");
	texCache.load<TextureLoader>("terrain"_hs, TEX::Builder().mipmapLinear().rgba().buildTextureAtlas("./res/textures/terrain.png", 16, 16));

	auto shader = ss.load("basic"_hs, "basic.vert", "basic.frag");
	auto chunk_shader = ss.load("chunk"_hs, "chunk_shader.vert", "chunk_shader.frag");
		
	const auto& window = DVZ::Window::getInstance();
	P = glm::perspectiveFov<float>(80.0f, (float)window.getWidth(), (float)window.getHeight(), .1f, 100000.0f);
	V = glm::identity<glm::mat4>();
	VP = P * V;

	glEnable(GL_DEPTH_TEST);
}

void BasicRenderer::prerender(const PerspectiveCamera& camera) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CCW);
	
	const auto& window = DVZ::Window::getInstance();
	P = glm::perspectiveFov<float>((float)glm::radians(80.0f), (float)window.getWidth(), (float)window.getHeight(), camera.near, camera.far);


	V = glm::lookAt(camera.pos, camera.rot * glm::vec3(0, 0, -1) + camera.pos, glm::vec3(0, 1, 0));

	VP = P * V;
}

void BasicRenderer::render(const InterpolatedScene& scene, const Voxel::ChunkRenderDataManager& chunkManager) {
	using namespace entt;

	prerender(scene.playerCamera);

	auto shader = ss.get("basic"_hs);
	auto tex = texCache.handle("uv"_hs);

	shader->start();
	for (const auto& instance : scene.instances) {
		auto mesh = meshCache.handle(instance.meshID);
		glm::mat4 M = instance.transform;

		shader->setUniformMat4("MVP", VP * M);
		shader->setUniform1i("diffuse", 0);

		tex->bindActiveTexture(0);

		mesh->vao.bind();
		mesh->ebo.bind();
		glDrawElements(GL_TRIANGLES, (GLsizei)(mesh->ebo.getNumBytes() / sizeof(u32)), GL_UNSIGNED_INT, 0);
		mesh->ebo.unbind();
		mesh->vao.unbind();
	}
	

	tex->unbind();


	shader->end();

	auto terrain = texCache.handle("terrain"_hs);
	terrain->bindActiveTexture(0);
	auto chunk_shader = ss.get("chunk"_hs);
	chunk_shader->start();
	for (const auto& [coords, chunk] : chunkManager.getRenderableChunks()) {
		const Graphics::VAO& vao = chunk.getVAO();
		const Graphics::VBO& ebo = chunk.getEBO();

		glm::vec3 chunk_world_pos = glm::vec3(coords) * Voxel::BLOCK_WIDTH * glm::vec3(Voxel::CHUNK_X, Voxel::CHUNK_Y, Voxel::CHUNK_Z);

		chunk_shader->setUniform1i("u_tex_atlas", 0);
		chunk_shader->setUniform3f("u_pos", chunk_world_pos);
		chunk_shader->setUniformMat4("VP", VP);
		vao.bind();
		ebo.bind();
		glDrawElements(GL_TRIANGLES, (GLsizei)(ebo.getNumBytes() / sizeof(Voxel::ChunkRenderData::EBOIndexType)), GL_UNSIGNED_INT, 0);
		ebo.unbind();
		vao.unbind();
		
	}
	chunk_shader->end();
	terrain->unbind();

}