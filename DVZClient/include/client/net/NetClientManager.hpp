#ifndef DVZ_NET_CLIENT_MANAGER_HPP
#define DVZ_NET_CLIENT_MANAGER_HPP

#include "client/net/ClientSocket.hpp"

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


	private:
		void update();
		std::unique_ptr<Net::ClientSocket> netclient = nullptr;
	};
}

#endif