#pragma once
#ifndef DVZ_NET_SYSTEM_HPP
#define DVZ_NET_SYSTEM_HPP

#include "steam/steamnetworkingtypes.h"

#include <memory>
#include <string_view>

namespace DVZ {
	class Engine;

	class ServerNetRecvSystem {
	public:
		ServerNetRecvSystem();
		void init(Engine& engine);
		void tick(Engine& engine);
	private:

		void onMessage(Engine&, std::string_view sv, HSteamNetConnection connection);
		void onClientJoin(Engine&, std::string_view sv, HSteamNetConnection connection);
		void onPlayerPositionVel(Engine&, std::string_view sv, HSteamNetConnection connection);
		void onPlayerInput(Engine&, std::string_view sv, HSteamNetConnection connection);
		//std::unique_ptr<Net::ServerSocket> socket;
		//void onMessage(std::string_view sv, HSteamNetConnection connection);
	};
}

#endif