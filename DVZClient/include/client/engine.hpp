#pragma once

#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include "client/systems/System.hpp"
#include "core/time.hpp"

#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <entt/entt.hpp>

namespace DVZ{

	namespace Graphics {
		class Scene;
		class SceneManager;
		class BasicRenderer;
	}

	namespace Voxel {
		class ClientChunkManager;
		class ChunkRenderDataManager;
	}


	namespace Net {
		class NetClientManager;
	}

	class Engine {
	public:
		Engine();
		~Engine();

		template<typename SYS>
		void addSystem() {
			systems.emplace_back(std::make_unique<SYS>());
			systems.back()->init(*this);
		}

		void update();
		void initUpdateLoop();
		void updateLoop();
		void render();
		void signalStop();

		entt::entity getPlayer();

		entt::registry& getRegistry();
		Graphics::Scene& getScene();
		Net::NetClientManager& getNetManager();
		Voxel::ClientChunkManager& getChunkManager();

		void setSimulationTime(duration duration);
		duration getSimulationTime() const;

		duration dt{ 1.0f / TPS };
	private:
		Voxel::ChunkRenderDataManager& getChunkRenderDataManager();



		std::atomic<bool> shouldStop = false;
		std::atomic<float> alpha = 0;
		//std::atomic<std::chrono::time_point<std::chrono::steady_clock>> last_update;

		entt::registry registry;
		std::unique_ptr<Graphics::Scene> scene;
		std::unique_ptr<Graphics::SceneManager> sceneManager;
		std::unique_ptr<Graphics::BasicRenderer> renderer;

		std::unique_ptr<Net::NetClientManager> netManager;

		std::unique_ptr<Voxel::ClientChunkManager> chunkManager;
		std::unique_ptr<Voxel::ChunkRenderDataManager> chunkRenderDataManager;
		std::thread updateThread;

		std::vector<std::unique_ptr<Systems::System>> systems;
		duration simulation_time = duration{ 0.0f };
	};
}

#endif