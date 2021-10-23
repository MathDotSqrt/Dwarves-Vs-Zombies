#pragma once
#ifndef DVZ_SERVER_SOCKET_HPP
#define DVZ_SERVER_SOCKET_HPP

#include "core/common.hpp"
#include "core/net/ConnectionState.hpp"

#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>
#include <string_view>
#include <unordered_map>

class ISteamNetworkingSockets;

namespace DVZ::Net {
	class ServerSocket {
	public: 
		ServerSocket(u16 port);
		~ServerSocket();

		void pollIncomingMessages();
		void sendMessage(HSteamNetConnection client, std::string_view sv);

	private:
		static ServerSocket* callbackInstance;
		static void connectionStatusCallback(SteamNetConnectionStatusChangedCallback_t* info);
		
		void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info);
		void pollConnectionStateChanges();

		std::unordered_map<HSteamNetConnection, ConnectionState> connections;

		HSteamListenSocket listenSock = 0;
		HSteamNetPollGroup pollGroup = 0;
		ISteamNetworkingSockets* socketsInterface = nullptr;
	};
}

#endif