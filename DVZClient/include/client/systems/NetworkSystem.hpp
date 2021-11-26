#pragma once
#ifndef DVZ_NETWORK_SYSTEM_HPP
#define DVZ_NETWORK_SYSTEM_HPP

#include "client/systems/System.hpp"

#include "core/net/Packet.hpp"

#include <memory>
#include <optional>

namespace DVZ::Systems {
	class NetworkSystem : public System {
	public:
		NetworkSystem();

		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	private:
		std::function<void(std::string_view)> func;
		void onMessage(Engine& engine, std::string_view data);

		void onAssignNetID(Engine& engine, std::string_view data);
		void onSyncSimulationClock(Engine& engine, std::string_view data);
		void onEntityPositionVel(Engine& engine, std::string_view data);
		void onPlayerPositionAck(Engine& engine, std::string_view data);
		void onPlayerJoin(Engine& engine, std::string_view data);
		void onSpawnPosition(Engine& engine, std::string_view data);

		//std::unique_ptr<Net::ClientSocket> netclient = nullptr;
	};
}

#endif 