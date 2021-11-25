#ifndef DVZ_SERVER_SEND_SYSTEM_HPP
#define DVZ_SERVER_SEND_SYSTEM_HPP

namespace DVZ {
	class Engine;

	class ServerNetSendSystem {
	public:
		ServerNetSendSystem();
		void init(Engine& engine);
		void tick(Engine& engine);
	};
}

#endif 