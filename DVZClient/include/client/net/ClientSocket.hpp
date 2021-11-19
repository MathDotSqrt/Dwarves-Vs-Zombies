#pragma once
#ifndef DVZ_CLIENT_SOCKET_HPP
#define DVZ_CLIENT_SOCKET_HPP

#include "core/net/ConnectionState.hpp"
#include "core/net/Packet.hpp"

#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>
#include <string_view>
#include <functional>

class ISteamNetworkingSockets;
struct SteamNetConnectionStatusChangedCallback_t;

namespace DVZ::Systems{
	class NetworkSystem;
}

namespace DVZ::Net {
	class ClientSocket {
	public:
		ClientSocket();
		ClientSocket(const std::string& ip);
		~ClientSocket();

		bool connectToServer(const std::string& ip);
		void pollIncommingMessages(const std::function<void(std::string_view)>& func);
		void sendMessage(std::string_view sv);
		
		bool isValid() const;
		ConnectionState getConnectionState() const;
	private:
		HSteamNetConnection connection = k_HSteamNetConnection_Invalid;
		ISteamNetworkingSockets* socketInterface = nullptr;
		ConnectionState connectionState = ConnectionState::INVALID;
		static ClientSocket* callbackInstance;

		void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);
		static void connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
		void pollConnectionStateChanges();
	};
}

#endif