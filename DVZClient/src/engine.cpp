#include "client/engine.hpp"
#include "client/components.hpp"
#include "client/window.hpp"

#include "client/graphics/scene.hpp"
#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/GeometryBuilder.hpp"

#include "client/systems/InputSystem.hpp"
#include "client/systems/MovementSystem.hpp"
#include "client/systems/RenderSystem.hpp"
#include "client/systems/VoxelSystem.hpp"

#include "client/voxel/ClientChunkManager.hpp"
#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/util/transform.hpp"

#include <chrono>
#include <spdlog/spdlog.h>

using namespace DVZ;

Engine::Engine() : 
	scene(std::make_unique<Graphics::Scene>()), 
	sceneManager(std::make_unique<Graphics::SceneManager>()),
	renderer(std::make_unique<Graphics::BasicRenderer>()),
	chunkManager(std::make_unique<Voxel::ClientChunkManager>()),
	chunkRenderDataManager(std::make_unique<Voxel::ChunkRenderDataManager>()),
	updateThread(&Engine::updateLoop, this){
	using namespace entt;

	entt::entity player = registry.create();
	registry.emplace<Transformation>(player);
	registry.emplace<Velocity>(player);
	registry.emplace<MovementState>(player);
	registry.emplace<Camera>(player, DVZ::Util::create_default_camera());
	registry.emplace<Player>(player);
	registry.emplace<Input>(player);
	registry.emplace<Direction>(player);

	entt::entity test = registry.create();
	registry.emplace<Transformation>(test);
	registry.emplace<Velocity>(test, glm::vec3(.0, .1, 0));
	registry.emplace<Renderable>(test, "cube"_hs);

	entt::entity test2 = registry.create();
	registry.emplace<Transformation>(test2, glm::vec3(1, 0, -10));
	//registry.emplace<Velocity>(test2, glm::vec3(0, 0, 2));
	//registry.emplace<Renderable>(test2, "cube"_hs);

	entt::entity test3 = registry.create();
	registry.emplace<Transformation>(test3, glm::vec3(0, 0, 0));
	//registry.emplace<Velocity>(test3, glm::vec3(1, 0, -1));
	//registry.emplace<Renderable>(test3, "cube"_hs);

}

Engine::~Engine() {
	signalStop();

	updateThread.join();
}

void Engine::update(duration total_time) {
	for (auto& system : systems) {
		system->gameTick(*this);
	}
}

void Engine::initUpdateLoop() {
	this->addSystem<Systems::InputSystem>();
	this->addSystem<Systems::MovementSystem>();
	this->addSystem<Systems::VoxelSystem>();
	this->addSystem<Systems::RenderSystem>();
}

void Engine::updateLoop() {
	using namespace DVZ;

	initUpdateLoop();
	spdlog::info("Engine: launched update loop with ticks per second: [{}]", TPS);

	std::chrono::time_point current_time = std::chrono::steady_clock::now();
	duration accum{0};
	duration t{ 0.0 };
	while (!shouldStop) {
		std::chrono::time_point new_time = std::chrono::steady_clock::now();
		//TODO: clamp frametime?? https://gafferongames.com/post/fix_your_timestep/
		duration frame_time = new_time - current_time;
		current_time = new_time;

		accum += frame_time;

		while (accum >= dt) {
			//last_update = std::chrono::steady_clock::now();
			update(t);

			chunkRenderDataManager->bufferDirtyChunks(*chunkManager);
			sceneManager->bufferScene(*scene);
			t += dt;
			accum -= dt;

			//This is a bug, it is possible for alpha to be larger than 1
			this->alpha = accum / dt;
		}

		this->alpha = accum / dt;
	}
}

void Engine::render() {
	//std::chrono::time_point last = last_update.load();
	//std::chrono::time_point now = std::chrono::steady_clock::now();
	//duration delta = now - last;
	//float alpha = std::min(delta / dt, 1.0f);
	//spdlog::debug("{}", alpha.load());
	sceneManager->computeInterpolate(alpha.load());
	chunkRenderDataManager->update();
	renderer->render(sceneManager->getInterpolatedScene(), *chunkRenderDataManager);
}

void Engine::signalStop() {
	shouldStop = true;
}

entt::entity Engine::getPlayer() {
	return *registry.view<Player>().begin();
}

entt::registry& Engine::getRegistry() {
	return registry;
}

Graphics::Scene& Engine::getScene() {
	return *scene;
}

Voxel::ClientChunkManager& Engine::getChunkManager() {
	return *chunkManager;
}

Voxel::ChunkRenderDataManager& Engine::getChunkRenderDataManager() {
	return *chunkRenderDataManager;
}