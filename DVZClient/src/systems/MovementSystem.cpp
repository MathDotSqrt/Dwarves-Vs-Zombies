#include "client/systems/MovementSystem.hpp"

#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "core/MovementUtil.hpp"

#include "core/CoreComponents.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void MovementSystem::init(Engine& engine) {
	spdlog::info("Initalized [MovementSystem]");
}

void MovementSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();


	auto camera_view = registry.view<Input, Transformation, Camera, Direction>();
	camera_view.each([&](Input& input, Transformation& transform, Camera& camera, Direction& dir) {
		glm::vec2 delta = input.current_cursor_pos - input.last_cursor_pos;
		transform.rot = rotatePlayer(delta, transform.rot, dir);
	});


	auto movement_view = registry.view<MovementState, Velocity, Transformation, Direction>();
	movement_view.each([](MovementState& state, Velocity& vel, Transformation& transform, Direction& dir) {
		vel = computePlayerVelocity(state, transform.rot, dir);
	});
}