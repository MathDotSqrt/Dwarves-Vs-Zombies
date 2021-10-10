#pragma once
#ifndef DVZ_SERVER_SOCKET_HPP
#define DVZ_SERVER_SOCKET_HPP

#include "core/common.hpp"
#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>

class ISteamNetworkingSockets;

namespace DVZ::Net {
	class ServerSocket {
	public: 
		ServerSocket(u16 port);
		~ServerSocket();

		void pollIncomingMessages();

	private:
		static ServerSocket* callbackInstance;
		static void connectionStatusCallback(SteamNetConnectionStatusChangedCallback_t* info);
		
		void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info);
		void pollConnectionStateChanges();

		HSteamListenSocket listenSock = 0;
		HSteamNetPollGroup pollGroup = 0;
		ISteamNetworkingSockets* socketsInterface = nullptr;
	};
}

#endif