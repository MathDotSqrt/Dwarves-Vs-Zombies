#pragma once
#ifndef DVZ_SERVER_SOCKET_HPP
#define DVZ_SERVER_SOCKET_HPP

#include "core/common.hpp"
#include "core/net/ConnectionState.hpp"
#include "core/net/Packet.hpp"

#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>
#include <string_view>
#include <unordered_map>
#include <functional>

class ISteamNetworkingSockets;

namespace DVZ::Net {
	class ServerSocket {
	public: 
		ServerSocket(u16 port);
		~ServerSocket();

		void pollIncomingMessages(const std::function<void(std::string_view,HSteamNetConnection)>& func);
		void sendMessage(std::string_view data, HSteamNetConnection client);
		void sendToAllMessage(std::string_view data, HSteamNetConnection except = k_HSteamNetConnection_Invalid);

		template<typename Packet>
		void sendMessage(const Packet& packet, HSteamNetConnection client) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{bytes.data(), bytes.size()};
			sendMessage(sv, client);
		}

		template<typename Packet>
		void sendToAllMessage(const Packet& packet, HSteamNetConnection except = k_HSteamNetConnection_Invalid) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			sendToAllMessage(sv, client);
		}

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