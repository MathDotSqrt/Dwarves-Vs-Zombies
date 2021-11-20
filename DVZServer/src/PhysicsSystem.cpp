#include "server/PhysicsSystem.hpp"
#include "server/Engine.hpp"
#include "core/CoreComponents.hpp"

using namespace DVZ;

PhysicsSystem::PhysicsSystem(Engine& engine) {

}

void PhysicsSystem::tick(Engine& engine) {
	auto& registry = engine.getRegistry();
	
	auto view = registry.view<Transformation, Velocity>();
	view.each([](Transformation& transform, Velocity& vel) {
		transform.pos = transform.pos + vel;
	});
}