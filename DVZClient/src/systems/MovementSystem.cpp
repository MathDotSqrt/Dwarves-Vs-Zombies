#include "client/systems/MovementSystem.hpp"

#include "client/engine.hpp"
#include "client/components.hpp"
#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void MovementSystem::init(Engine& engine) {
	spdlog::info("Initalized [MovementSystem]");
}

void MovementSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();

	auto movement_view = registry.view<MovementState, Velocity, Transformation, Direction>();
	movement_view.each([](MovementState& state, Velocity& vel, Transformation& transform, Direction& dir) {
		glm::vec3 forward_vel = state.forward * (transform.rot * dir.forward);
		glm::vec3 strafe_vel = state.strafe * (transform.rot * dir.right);
		glm::vec3 fly_vel = state.fly * dir.up;

		vel =  6.0f * (forward_vel + strafe_vel + fly_vel);
	});


	auto view = registry.view<DVZ::Transformation, DVZ::Velocity>();
	for (const auto& e : view) {
		auto& trans = view.get<DVZ::Transformation>(e);
		const auto& vel = view.get<DVZ::Velocity>(e);

		trans.pos = trans.pos + vel * (1.0f / engine.TPS);
	}
}