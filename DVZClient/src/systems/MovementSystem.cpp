#include "client/systems/MovementSystem.hpp"

#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/util/transform.hpp"

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
		glm::quat q_yaw = glm::angleAxis(-delta.x / 110, glm::vec3(0, 1, 0));
		glm::quat q_pitch = glm::angleAxis(-delta.y / 140, glm::vec3(1, 0, 0));



		glm::quat new_rot = (q_yaw * transform.rot) * q_pitch;
		glm::quat move_dir = Util::remove_pitch(new_rot);

		if (glm::dot(new_rot * dir.forward, move_dir * dir.forward) < .01f) {
			transform.rot = q_yaw * transform.rot;
		}
		else {
			transform.rot = new_rot;
		}

	});


	auto movement_view = registry.view<MovementState, Velocity, Transformation, Direction>();
	movement_view.each([](MovementState& state, Velocity& vel, Transformation& transform, Direction& dir) {
		glm::quat move_dir = Util::remove_pitch(transform.rot);
		
		glm::vec3 forward_vel = state.forward * (move_dir * dir.forward);
		glm::vec3 strafe_vel = state.strafe * (move_dir * dir.right);
		glm::vec3 fly_vel = state.fly * dir.up;

		vel =  20.0f * (forward_vel + strafe_vel + fly_vel);
	});


	auto view = registry.view<DVZ::Transformation, DVZ::Velocity>();
	for (const auto& e : view) {
		auto& trans = view.get<DVZ::Transformation>(e);
		const auto& vel = view.get<DVZ::Velocity>(e);

		trans.pos = trans.pos + vel * (1.0f / engine.TPS);
	}
}