#include "client/systems/InputSystem.hpp"

#include "client/window.hpp"
#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "core/CoreComponents.hpp"

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
			movement.forward += 1;
		}
		if (window.isPressed('s')) {
			movement.forward -= 1;
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

	auto input_view = registry.view<Input>();
	input_view.each([pos](Input& input) {
		input.last_cursor_pos = input.current_cursor_pos;
		input.current_cursor_pos = pos;
	});
}