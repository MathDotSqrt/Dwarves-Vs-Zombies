#include "client/systems/ClientPredictionSystem.hpp"

#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/predict_client.hpp"

#include "core/MovementUtil.hpp"

#include "core/CoreComponents.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void ClientPredictionSystem::init(Engine& engine) {
	spdlog::info("Initalized [MovementSystem]");
}

void ClientPredictionSystem::gameTick(Engine& engine) {
	auto& registry = engine.getRegistry();


	auto camera_view = registry.view<Input, Transformation, Camera, Direction>();
	camera_view.each([&](Input& input, Transformation& transform, Camera& camera, Direction& dir) {
		glm::vec2 delta = input.current_cursor_pos - input.last_cursor_pos;
		transform.rot = rotatePlayer(delta, transform.rot, dir);
	});


	entt::entity player = engine.getPlayer();
	const auto& state = registry.get<MovementState>(player);
	auto& transform = registry.get<Transformation>(player);
	auto& vel = registry.get<Velocity>(player);
	DVZ::ClientPlayerState new_pos_vel = DVZ::predict_client_player_state(engine, state, transform);

	transform = new_pos_vel.transform;
	vel = new_pos_vel.velocity;
}