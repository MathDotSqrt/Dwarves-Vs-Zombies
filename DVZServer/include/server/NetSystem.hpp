#pragma once
#ifndef DVZ_NET_SYSTEM_HPP
#define DVZ_NET_SYSTEM_HPP

#include "steam/steamnetworkingtypes.h"

#include <memory>
#include <string_view>

namespace DVZ::Net {
	class ServerSocket;
}

namespace DVZ {
	class Engine;

	class NetSystem {
	public:
		NetSystem(Engine& engine);
		void tick(Engine& engine);
	private:



		std::unique_ptr<Net::ServerSocket> socket;
		//void onMessage(std::string_view sv, HSteamNetConnection connection);
	};
}

#endif