#pragma once
#ifndef DVZ_PHYSICS_SYSTEM_HPP
#define DVZ_PHYSICS_SYSTEM_HPP

#include "client/systems/System.hpp"

namespace DVZ::Systems {
	class PhysicsSystem : public System {
	public:
		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	};
}

#endif 