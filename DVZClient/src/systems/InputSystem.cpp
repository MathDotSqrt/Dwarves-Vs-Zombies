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


	auto view = registry.view<Input, MovementState>();
	for (auto entity : view) {
		MovementState& movement = view.get<MovementState>(entity);

		movement.forward = 0;
		movement.strafe = 0;
		movement.fly = 0;

		if (window.isDown('w')) {
			movement.forward += 1;
		}
		if (window.isDown('s')) {
			movement.forward -= 1;
		}

		if (window.isDown('d')) {
			movement.strafe += 1;
		}
		if (window.isDown('a')) {
			movement.strafe -= 1;
		}

		if (window.isDown(' ')) {
			movement.fly += 1;
		}
		if (window.isDown(Window::Keys::LEFT_SHIFT)) {
			movement.fly -= 1;
		}
	}

	auto input_view = registry.view<Input>();
	input_view.each([&](Input& input) {
		input.last_cursor_pos = input.current_cursor_pos;
		input.current_cursor_pos = window.getMousePos();
		input.left_click = window.isDown(Window::Mouse::LEFT_CLICK);
		input.right_click = window.isDown(Window::Mouse::RIGHT_CLICK);
	});
}