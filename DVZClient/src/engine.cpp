#include "client/engine.hpp"
#include "client/ClientComponents.hpp"
#include "client/window.hpp"

#include "client/net/ClientSocket.hpp"
#include "client/net/NetClientManager.hpp"

#include "client/graphics/Scene.hpp"
#include "client/graphics/BasicRenderer.hpp"
#include "client/graphics/GeometryBuilder.hpp"

#include "client/systems/InputSystem.hpp"
#include "client/systems/ClientPredictionSystem.hpp"
#include "client/systems/RenderSystem.hpp"
#include "client/systems/VoxelSystem.hpp"
#include "client/systems/PhysicsSystem.hpp"
#include "client/systems/NetworkSystem.hpp"

#include "client/voxel/ClientChunkManager.hpp"
#include "client/voxel/ChunkRenderDataManager.hpp"

#include "client/util/camera_util.hpp"
#include "core/util/transform.hpp"
#include "core/CoreComponents.hpp"

#include <chrono>
#include <spdlog/spdlog.h>

using namespace DVZ;

Engine::Engine(std::string_view ip) : 
	scene(std::make_unique<Graphics::Scene>()), 
	sceneManager(std::make_unique<Graphics::SceneManager>()),
	renderer(std::make_unique<Graphics::BasicRenderer>()),
	netManager(std::make_unique<Net::NetClientManager>()),
	chunkManager(std::make_unique<Voxel::ClientChunkManager>()),
	chunkRenderDataManager(std::make_unique<Voxel::ChunkRenderDataManager>()),
	updateThread(&Engine::updateLoop, this){
	using namespace entt;

	setIP(ip);
}

Engine::~Engine() {
	signalStop();

	updateThread.join();
}

void Engine::update() {
	auto& window = Window::getInstance();
	if (window.isDown('1')) {
		spdlog::set_level(spdlog::level::debug);
	}
	if (window.isDown('2')) {
		spdlog::set_level(spdlog::level::info);
	}
	if (window.isDown('3')) {
		spdlog::set_level(spdlog::level::err);
	}

	if (window.isPressed('r')) {
		spdlog::info("Reset");
		desync = true;
		pause = false;
		client_simulation_time = simulation_duration{0};
	}

	if (window.isPressed('t')) {
		spdlog::info("Continue");
		desync = false;
		pause = false;
	}

	if (window.isPressed('l') || window.isDown('p')) {
		desync = true;
		pause = true;
		client_simulation_time += simulation_duration{ 1 };
	}
	if (window.isPressed('j') || window.isDown('i')) {
		desync = true;
		pause = true;
		client_simulation_time -= simulation_duration{ 1 };
	}

	for (auto& system : systems) {
		system->gameTick(*this);
	}
}

void Engine::initUpdateLoop() {
	using namespace entt;
	entt::entity player = registry.create();
	registry.emplace<Transformation>(player, glm::vec3(0, 20, -10));
	registry.emplace<Velocity>(player);
	registry.emplace<MovementState>(player);
	registry.emplace<Camera>(player, DVZ::Util::create_default_camera());
	registry.emplace<Player>(player);
	registry.emplace<Input>(player);
	registry.emplace<Direction>(player);
	registry.emplace<VoxelCollider>(player, Collision::AABB{ glm::vec3(-.3, -1.5, -.3), glm::vec3(.3, .5, .3) });
	//registry.emplace<Network>(player);

	this->addSystem<Systems::InputSystem>();
	this->addSystem<Systems::ClientPredictionSystem>();
	//this->addSystem<Systems::VoxelSystem>();
	this->addSystem<Systems::NetworkSystem>();
	this->addSystem<Systems::RenderSystem>();
}

void Engine::updateLoop() {
	using namespace DVZ;

	initUpdateLoop();
	spdlog::info("Engine: launched update loop with ticks per second: [{}]", TPS);

	std::chrono::time_point start_time = std::chrono::steady_clock::now();
	std::chrono::time_point current_time = start_time;
	duration accum{0};
	while (!shouldStop) {
		std::chrono::time_point new_time = std::chrono::steady_clock::now();
		//TODO: clamp frametime?? https://gafferongames.com/post/fix_your_timestep/
		duration frame_time = new_time - current_time;
		current_time = new_time;

		accum += frame_time;

		while (accum >= DT) {
			//last_update = std::chrono::steady_clock::now();
			if(pause == false)
				client_simulation_time += DT;
			update();

			chunkRenderDataManager->bufferDirtyChunks(*chunkManager);
			sceneManager->bufferScene(*scene);
			accum -= DT;

			//This is a bug, it is possible for alpha to be larger than 1
			this->alpha = accum / DT;
		}

		this->alpha = accum / DT;
	}
}

void Engine::render() {
	//std::chrono::time_point last = last_update.load();
	//std::chrono::time_point now = std::chrono::steady_clock::now();
	//duration delta = now - last;
	//float alpha = std::min(delta / dt, 1.0f);
	//spdlog::debug("{}", alpha.load());
	sceneManager->computeInterpolate(alpha.load());

	if (Window::getInstance().isDown('c'))
		chunkRenderDataManager->clearRenderData();
	chunkRenderDataManager->update();
	renderer->render(sceneManager->getInterpolatedScene(), *chunkRenderDataManager);
}

void Engine::signalStop() {
	shouldStop = true;
}

entt::entity Engine::getPlayer() const {
	return registry.view<const Player>().front();
}

entt::registry& Engine::getRegistry() {
	return registry;
}

const entt::registry& Engine::getRegistry() const {
	return registry;
}


Graphics::Scene& Engine::getScene() {
	return *scene;
}

Net::NetClientManager& Engine::getNetManager() {
	return *netManager;
}

Voxel::ClientChunkManager& Engine::getChunkManager() {
	return *chunkManager;
}

const Voxel::ClientChunkManager& Engine::getChunkManager() const {
	return *chunkManager;
}

Voxel::ChunkRenderDataManager& Engine::getChunkRenderDataManager() {
	return *chunkRenderDataManager;
}

void Engine::setSimulationTime(simulation_duration server_time) {
	if (server_time > last_server_time && desync == false) {
		last_server_time = server_time;
		sync_offset = last_server_time - client_simulation_time;
	}
}


simulation_duration Engine::getServerSimulationTime() const{
	return client_simulation_time + sync_offset;
}

simulation_duration Engine::getClientSimulationTime() const {
	return client_simulation_time;
}

void Engine::setIP(std::string_view sv) {
	ip = sv;
	getNetManager().connectTo(ip);
}