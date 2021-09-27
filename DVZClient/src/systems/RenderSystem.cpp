#include "client/systems/RenderSystem.hpp"

#include "client/engine.hpp"
#include "client/components.hpp"
#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

RenderSystem::RenderSystem() {

}

void RenderSystem::init(Engine& engine) {
	spdlog::info("Initalized [RenderSystem]");
}

void RenderSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	auto view = registry.view<Transformation, Renderable>();
	for (const auto& e : view) {
		const auto& trans = view.get<DVZ::Transformation>(e);
		auto& render = view.get<DVZ::Renderable>(e);

		if (!scene.hasInstance(render.instance_id)) {
			render.instance_id = scene.addInstance(render.mesh_id);
		}

		Graphics::Instance& instance = scene.getInstance(render.instance_id);
		instance.pos = trans.pos;
		instance.rot = trans.rot;
		instance.scale = trans.scale;
	}

	auto player_camera_view = registry.view<Transformation, Camera, Player>();
	player_camera_view.each([&](Transformation& transform, Camera& camera) {
		Graphics::PerspectiveCamera& playerSceneCamera = scene.getPlayerCamera();
		playerSceneCamera.near = camera.near;
		playerSceneCamera.far = camera.far;
		playerSceneCamera.height = camera.height;
		playerSceneCamera.width = camera.width;
		playerSceneCamera.fov = camera.fov;
		playerSceneCamera.pos = transform.pos;
		playerSceneCamera.rot = transform.rot;
		playerSceneCamera.scale = transform.scale;
		

	});
}