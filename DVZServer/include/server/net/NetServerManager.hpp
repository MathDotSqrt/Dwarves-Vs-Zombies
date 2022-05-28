#ifndef DVZ_NET_PLAYER_MANAGER_HPP
#define DVZ_NET_PLAYER_MANAGER_HPP


#include "server/net/ServerSocket.hpp"
#include "server/net/EntitySnapshot.hpp"

#include "core/net/Packet.hpp"

#include <steam/steamnetworkingtypes.h>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <entt/entity/entity.hpp>

namespace DVZ::Net {

	class NetServerManager {
	public:
		NetServerManager();

		template<typename FUNC>
		void poll(FUNC func) {
			if (socket) {
				socket->pollIncomingMessages(func);

				auto connections = socket->getInvalidConnections();
				for (auto& connection : connections) {
					std::string buffer;
					buffer.push_back(static_cast<char>(PacketID::SB_ClientDisconnected));
					func(buffer, connection);
				}
				socket->removeInvalidConnections();
			}
			
		}

		template<typename Packet>
		inline void sendMessage(const Packet& packet, HSteamNetConnection client, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendMessage(sv, client, reliable);
		}

		template<typename Packet>
		inline void sendToAllMessage(const Packet& packet, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendToAllMessage(sv, k_HSteamNetConnection_Invalid, reliable);
		}

		template<typename Packet>
		inline void sendToAllMessage(const Packet& packet, HSteamNetConnection except, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			socket->sendToAllMessage(sv, except, reliable);
		}

		void addEntityConnectionMapping(entt::entity, HSteamNetConnection conn);
		void removeEntityConnectionMapping(HSteamNetConnection conn);
		HSteamNetConnection getConnectionHandle(entt::entity) const;
		entt::entity getEntity(HSteamNetConnection conn) const;

		bool setEntityInputTime(entt::entity entity, simulation_duration client_time);
		std::optional<simulation_duration> shouldAckEntityInput(entt::entity entity);

		void setEntitySnapshot(std::shared_ptr<EntitySnapshot> master);
		void ackClientSnapshot(HSteamNetConnection client, simulation_duration ack_simulation_time);
		EntitySnapshotDelta getClientSnapshotDelta(HSteamNetConnection client) const;
	private: 
		void update();

		std::unique_ptr<Net::ServerSocket> socket;
		std::unordered_map<entt::entity, HSteamNetConnection> entityToConnectionMap;
		std::unordered_map<HSteamNetConnection, entt::entity> connectionToEntityMap;
	
		std::unordered_map<entt::entity, std::pair<simulation_duration, bool>> entityLastUpdateTime;

		static const EntitySnapshot zeroSnapshot;
		std::shared_ptr<EntitySnapshot> masterGamestate;

		std::unordered_map<HSteamNetConnection, std::vector<std::shared_ptr<EntitySnapshot>>> clientSnapshots;
	};
}

#endif
