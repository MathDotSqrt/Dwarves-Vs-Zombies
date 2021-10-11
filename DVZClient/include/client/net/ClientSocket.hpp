#pragma once
#ifndef DVZ_CLIENT_SOCKET_HPP
#define DVZ_CLIENT_SOCKET_HPP

#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>
#include <string_view>

class ISteamNetworkingSockets;
struct SteamNetConnectionStatusChangedCallback_t;

namespace DVZ::Net {

	class ClientSocket {
	public:
		ClientSocket(std::string_view ip);
		~ClientSocket();

		void pollIncommingMessages();
	private:
		HSteamNetConnection connection = 0;
		ISteamNetworkingSockets* socketInterface = nullptr;
		static ClientSocket* callbackInstance;

		void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);
		static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
		void pollConnectionStateChanges();
	};
}

#endif