#pragma once
#ifndef DVZ_NETWORK_SYSTEM_HPP
#define DVZ_NETWORK_SYSTEM_HPP

#include "client/systems/System.hpp"

#include "client/net/ClientSocket.hpp"

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

		void processMessage(std::string_view message);
		std::unique_ptr<Net::ClientSocket> netclient = nullptr;
	};
}

#endif 