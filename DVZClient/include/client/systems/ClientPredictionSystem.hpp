#pragma once
#ifndef DVZ_MOVEMENT_SYSTEM_HPP
#define DVZ_MOVEMENT_SYSTEM_HPP

#include "client/systems/System.hpp"

namespace DVZ::Systems {
	class ClientPredictionSystem : public System {
		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	};
}

#endif 