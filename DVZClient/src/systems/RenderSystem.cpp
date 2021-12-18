#include "client/systems/RenderSystem.hpp"

#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/graphics/Scene.hpp"

#include "core/CoreComponents.hpp"

#include <spdlog/spdlog.h>


#undef near
#undef far

using namespace DVZ::Systems;

RenderSystem::RenderSystem() {

}

void RenderSystem::init(Engine& engine) {
	spdlog::info("Initalized [RenderSystem]");

	auto& registry = engine.getRegistry();

	registry.on_destroy<Renderable>().connect<&RenderSystem::onDeleteInstance>(this);
}

void RenderSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	for (const auto& id : removeInstanceBuffer) {
		scene.removeInstance(id);
	}
	removeInstanceBuffer.clear();

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
		Graphics::PerspectiveCamera playerSceneCamera;
		playerSceneCamera.near = camera.near;
		playerSceneCamera.far = camera.far;
		playerSceneCamera.height = camera.height;
		playerSceneCamera.width = camera.width;
		playerSceneCamera.fov = camera.fov;
		playerSceneCamera.pos = transform.pos;
		playerSceneCamera.rot = transform.rot;
		playerSceneCamera.scale = transform.scale;

		scene.setPlayerCamera(playerSceneCamera);

	});
}

void RenderSystem::onDeleteInstance(entt::registry& registry, entt::entity entity) {
	const auto& renderable = registry.get<const Renderable>(entity);
	removeInstanceBuffer.push_back(renderable.instance_id);
}