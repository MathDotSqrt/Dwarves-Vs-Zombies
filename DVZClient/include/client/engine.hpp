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
		Engine(std::string_view ip);
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

		void setSimulationTime(simulation_duration duration);
		simulation_duration getSimulationTime() const;

		void setIP(std::string_view sv);
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

		bool sync_time = false;
		simulation_duration last_server_time = simulation_duration{ 0 };
		simulation_duration simulation_time = simulation_duration{ 0 };

		std::string ip = "127.0.0.1:50150";
	};
}

#endif