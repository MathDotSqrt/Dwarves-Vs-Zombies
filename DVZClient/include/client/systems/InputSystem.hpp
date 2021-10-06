#pragma once
#ifndef DVZ_INPUT_SYSTEM_HPP
#define DVZ_INPUT_SYSTEM_HPP

#include "client/systems/System.hpp"

namespace DVZ::Systems {
	class InputSystem : public System {
		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	};
}

#endif