#ifndef DVZ_NET_PLAYER_MANAGER_HPP
#define DVZ_NET_PLAYER_MANAGER_HPP

#include "steam/steamnetworkingtypes.h"
#include "server/net/ServerSocket.hpp"
#include <memory>
#include <string_view>

namespace DVZ::Net {

	class NetServerManager {
	public:
		NetServerManager();


		template<typename FUNC>
		void poll(FUNC func) {
			socket->pollIncomingMessages(func);
		}

		template<typename Packet>
		void sendMessage(const Packet& packet, HSteamNetConnection client) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendMessage(sv, client);
		}

		template<typename Packet>
		void sendToAllMessage(const Packet& packet, HSteamNetConnection except = k_HSteamNetConnection_Invalid) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendToAllMessage(sv, client);
		}

		Net::ServerSocket& getSocket();

	private:
		std::unique_ptr<Net::ServerSocket> socket;
	};
}

#endif