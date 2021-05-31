#pragma once

#ifndef DVZ_SYSTEM_HPP
#define DVZ_SYSTEM_HPP

namespace DVZ {
	class Engine;
}

namespace DVZ::Systems {
	class System {
	public:
		virtual ~System() {}

		virtual void init(Engine& engine) {}

		virtual void gameTick(Engine& engine) = 0;
		virtual void update(Engine& engine) {};
	};
}

#endif