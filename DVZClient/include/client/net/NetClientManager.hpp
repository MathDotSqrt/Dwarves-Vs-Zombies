#ifndef DVZ_NET_CLIENT_MANAGER_HPP
#define DVZ_NET_CLIENT_MANAGER_HPP

#include "client/net/ClientSocket.hpp"
#include "client/net/InterpolateNetValues.hpp"
#include "core/CoreComponents.hpp"
#include <vector>
#include <optional>
namespace DVZ::Net {

	struct Request {
		MovementState input;
		Transformation transform;
		simulation_duration client_time;
	};

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
		void removeServerIDMap(entt::entity server);
		entt::entity getClientID(entt::entity server) const;

		void appendRequest(const MovementState& input, const Transformation& pos, simulation_duration client_time);
		bool ackRequest(const glm::vec3& pos, simulation_duration client_request_time);

		void appendEntityPositionValues(entt::entity clientID, const PositionNetValues& values, simulation_duration client_time);
		std::optional<PositionNetValues> getInterpolatedEntityValues(entt::entity clientID, simulation_duration client_time) const;

		std::vector<Request>& getUnackedRequests();
	private:

		void update();
		std::unique_ptr<Net::ClientSocket> netclient = nullptr;
		std::unordered_map<entt::entity, entt::entity> serverIDMap;
		std::unordered_map<entt::entity, InterpolateNetValues> entityInterpolationMapBuffer;

		std::vector<Request> unackedRequestBuffer;
	};
}

#endif