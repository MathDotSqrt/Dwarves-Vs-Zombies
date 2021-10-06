#pragma once

#ifndef DVZ_RENDER_SYSTEM_HPP
#define DVZ_RENDER_SYSTEM_HPP

#include "client/systems/System.hpp"

namespace DVZ::Systems {
	class RenderSystem : public System {
	public:
		RenderSystem();

		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	};
}

#endif