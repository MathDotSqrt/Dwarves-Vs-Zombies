#ifndef DVZ_NET_CLIENT_MANAGER_HPP
#define DVZ_NET_CLIENT_MANAGER_HPP

#include "client/net/ClientSocket.hpp"
#include "core/CoreComponents.hpp"
#include <vector>
#include <optional>
namespace DVZ::Net {

	class NetClientManager {
	public:
		NetClientManager();

		template<typename FUNC>
		void poll(FUNC func) {
			if (netclient->isValid()) {
				netclient->pollIncommingMessages(func);
				update();
			}
		}
		template<typename Packet>
		void sendMessage(const Packet& packet, bool reliable=true) {
			const auto& bytes = serializePacketData(packet);
			std::string_view sv{ bytes.data(), bytes.size() };
			netclient->sendMessage(sv, reliable);
		}
		void connectTo(std::string_view ip);
		bool hasServerID(entt::entity server) const;
		void addServerIDMap(entt::entity server, entt::entity client);
		entt::entity getClientID(entt::entity server) const;

		void appendRequest(const MovementState& input, const glm::vec3& pos, simulation_duration client_time);
		bool ackRequest(const glm::vec3& pos, simulation_duration client_request_time);
	private:

		struct Request {
			MovementState input;
			glm::vec3 pos;
			simulation_duration client_time;
		};

		void update();
		std::unique_ptr<Net::ClientSocket> netclient = nullptr;
		std::unordered_map<entt::entity, entt::entity> serverIDMap;

		std::vector<Request> unackedRequestBuffer;
	};
}

#endif