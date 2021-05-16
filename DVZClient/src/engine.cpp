#include "client/engine.hpp"
#include "client/graphics/GeometryBuilder.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
#include <client/components.hpp>
#include <client/window.hpp>

using namespace DVZ;

Engine::Engine() : updateThread(&Engine::updateLoop, this){
	using namespace entt;

	entt::entity test = registry.create();
	registry.emplace<DVZ::Transformation>(test);
	registry.emplace<DVZ::Velocity>(test, glm::vec3(0, 0, -2));
	registry.emplace<DVZ::Renderable>(test, "cube"_hs);

	entt::entity test2 = registry.create();
	registry.emplace<DVZ::Transformation>(test2, glm::vec3(1, 0, -10));
	registry.emplace<DVZ::Velocity>(test2, glm::vec3(0, 0, 2));
	registry.emplace<DVZ::Renderable>(test2, "cube"_hs);

	entt::entity test3 = registry.create();
	registry.emplace<DVZ::Transformation>(test3, glm::vec3(0, 0, 0));
	registry.emplace<DVZ::Velocity>(test3, glm::vec3(1, 0, -1));
	registry.emplace<DVZ::Renderable>(test3, "cube"_hs);
}

Engine::~Engine() {
	signalStop();

	updateThread.join();
}

void Engine::update(duration total_time) {
	if (DVZ::Window::getInstance().isPressed('c')) {
		auto view = registry.view<DVZ::Transformation, DVZ::Velocity>();
		for (auto& e : view) {
			auto& trans = view.get<DVZ::Transformation>(e);
			const auto& vel = view.get<DVZ::Velocity>(e);

			trans.pos += vel * dt.count();
		}
	}

	auto view = registry.view<DVZ::Transformation, DVZ::Renderable>();
	for (auto& e : view) {
		const auto& trans = view.get<DVZ::Transformation>(e);
		auto& render = view.get<DVZ::Renderable>(e);

		if (!scene.hasInstance(render.instance_id)) {
			render.instance_id = scene.addInstance(render.mesh_id);
		}

		Graphics::Instance& instance = scene.getInstance(render.instance_id);
		instance.pos = trans.pos;
		instance.rot = trans.rot;
		instance.scale = trans.scale;
	}
}

void Engine::updateLoop() {
	using namespace DVZ;

	spdlog::info("Engine: launched update loop with ticks per second: [{}]", 1 / dt.count());

	std::chrono::time_point current_time = std::chrono::steady_clock::now();
	duration accum{0};
	duration t{ 0.0 };
	while (!shouldStop) {
		std::chrono::time_point new_time = std::chrono::steady_clock::now();
		duration frame_time = new_time - current_time;
		current_time = new_time;

		accum += frame_time;

		while (accum >= dt) {
			last_update = std::chrono::steady_clock::now();
			update(t);
			sceneManager.bufferScene(scene);
			t += dt;
			accum -= dt;
		}

		float alpha = accum / dt;
	}
}

void Engine::render() {
	std::chrono::time_point last = last_update.load();
	std::chrono::time_point now = std::chrono::steady_clock::now();
	duration delta = now - last;
	float alpha = std::min(delta / dt, 1.0f);
	sceneManager.computeInterpolate(alpha);
	renderer.render(sceneManager.getInterpolatedScene());
}

void Engine::signalStop() {
	shouldStop = true;
}