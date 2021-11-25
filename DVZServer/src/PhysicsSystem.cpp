#include "server/PhysicsSystem.hpp"
#include "server/Engine.hpp"
#include "core/CoreComponents.hpp"
#include "core/MovementUtil.hpp"
#include "server/ServerComponents.hpp"

using namespace DVZ;

PhysicsSystem::PhysicsSystem(Engine& engine) {

}

void PhysicsSystem::tick(Engine& engine) {
	auto& registry = engine.getRegistry();
	
	auto movement_view = registry.view<MovementState, Velocity, Transformation, Direction>();
	movement_view.each([](MovementState& state, Velocity& vel, Transformation& transform, Direction& dir) {
		vel = computePlayerVelocity(state, transform.rot, dir);
	});

	auto debug_view = registry.view<Velocity, Debug>();

	debug_view.each([](Velocity& vel, Debug& debug) {
		debug.timer += .05f;
		vel.y = glm::sin(debug.timer) / 6;
		vel.x = glm::cos(debug.timer) / 6;
	});

	auto view = registry.view<Transformation, Velocity>();
	view.each([](Transformation& transform, Velocity& vel) {
		transform.pos = transform.pos + vel;
	});
}