#ifndef DVZ_NET_PLAYER_MANAGER_HPP
#define DVZ_NET_PLAYER_MANAGER_HPP

#include "steam/steamnetworkingtypes.h"
#include "server/net/ServerSocket.hpp"
#include <memory>
#include <string_view>
#include <unordered_map>
#include <entt/entity/entity.hpp>

namespace DVZ::Net {

	class NetServerManager {
	public:
		NetServerManager();

		template<typename FUNC>
		void poll(FUNC func) {
			if(socket)
				socket->pollIncomingMessages(func);
		}

		template<typename Packet>
		void sendMessage(const Packet& packet, HSteamNetConnection client, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendMessage(sv, client, reliable);
		}

		template<typename Packet>
		void sendToAllMessage(const Packet& packet, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendToAllMessage(sv, k_HSteamNetConnection_Invalid, reliable);
		}

		template<typename Packet>
		void sendToAllMessage(const Packet& packet, HSteamNetConnection except, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendToAllMessage(sv, except, reliable);
		}

		void addEntityConnectionMapping(entt::entity, HSteamNetConnection conn);
		HSteamNetConnection getConnectionHandle(entt::entity) const;
		entt::entity getEntity(HSteamNetConnection conn) const;

		bool shouldAcceptClientMovement(entt::entity, simulation_duration client_time);

		Net::ServerSocket& getSocket();

	private: 
		std::unique_ptr<Net::ServerSocket> socket;
		std::unordered_map<entt::entity, HSteamNetConnection> entityToConnectionMap;
		std::unordered_map<HSteamNetConnection, entt::entity> connectionToEntityMap;
		std::unordered_map<entt::entity, simulation_duration> clientLastUpdateMap;
	};
}

#endif