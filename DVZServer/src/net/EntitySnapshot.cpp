#include "server/net/EntitySnapshot.hpp"
#include "server/Engine.hpp"
#include "server/ServerComponents.hpp"
#include "core/CoreComponents.hpp"

using namespace DVZ::Net;

EntitySnapshot::EntitySnapshot(const Engine& engine) {
	const auto& registry = engine.getRegistry();
	
	auto view = registry.view<const NetPlayer, const Transformation>();

	for (entt::entity entity : view) {
		const auto& transform = view.get<const Transformation>(entity);
		EntityState state{transform.pos, transform.rot};
		
		entities[entity] = state;
	}
}