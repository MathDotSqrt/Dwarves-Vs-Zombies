#include "client/systems/MovementSystem.hpp"

#include "client/engine.hpp"
#include "client/components.hpp"

using namespace DVZ::Systems;

void MovementSystem::init(Engine& engine) {

}

void MovementSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();

	auto view = registry.view<DVZ::Transformation, DVZ::Velocity>();
	for (const auto& e : view) {
		auto& trans = view.get<DVZ::Transformation>(e);
		const auto& vel = view.get<DVZ::Velocity>(e);

		trans.pos = trans.pos + vel * (1.0f / engine.TPS);
		trans.rot *= glm::angleAxis(.01f, glm::vec3(1, 0, 0));
	}
}