#include "client/systems/RenderSystem.hpp"

#include "client/engine.hpp"
#include "client/components.hpp"

using namespace DVZ::Systems;

RenderSystem::RenderSystem() {

}

void RenderSystem::init(Engine& engine) {
	
}

void RenderSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();
	auto& scene = engine.getScene();

	auto view = registry.view<DVZ::Transformation, DVZ::Renderable>();
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
}