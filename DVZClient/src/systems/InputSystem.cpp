#include "client/systems/InputSystem.hpp"

#include "client/window.hpp"
#include "client/engine.hpp"
#include "client/components.hpp"

#include <spdlog/spdlog.h>

using namespace DVZ::Systems;

void InputSystem::init(Engine& engine) {
	spdlog::info("Initalized [InputSystem]");
}

void InputSystem::gameTick(Engine& engine) {
	auto& window = Window::getInstance();
	auto& registry = engine.getRegistry();

	glm::vec2 pos = window.getMousePos();

	auto view = registry.view<Input, MovementState>();
	for (auto entity : view) {
		MovementState& movement = view.get<MovementState>(entity);

		movement.forward = 0;
		movement.strafe = 0;
		movement.fly = 0;

		if (window.isPressed('w')) {
			movement.forward -= 1;
		}
		if (window.isPressed('s')) {
			movement.forward += 1;
		}

		if (window.isPressed('d')) {
			movement.strafe += 1;
		}
		if (window.isPressed('a')) {
			movement.strafe -= 1;
		}

		if (window.isPressed(' ')) {
			movement.fly += 1;
		}
		if (window.isPressed(Window::Keys::LEFT_SHIFT)) {
			movement.fly -= 1;
		}
	}

	auto camera_view = registry.view<Input, Transformation, Camera>();
	camera_view.each([&](Input& input, Transformation& transform, Camera& camera) {
		glm::vec2 delta = pos - input.last_cursor_pos;
		glm::quat q_yaw = glm::angleAxis(delta.x / 110, glm::vec3(0, 1, 0));
		glm::quat q_pitch = glm::angleAxis(delta.y / 140, glm::vec3(1, 0, 0));

		glm::quat new_rot = (q_yaw * transform.rot) * q_pitch;
		transform.rot = new_rot;
	});

	auto input_view = registry.view<Input>();
	input_view.each([pos](Input& input) {
		input.last_cursor_pos = pos;
	});
}