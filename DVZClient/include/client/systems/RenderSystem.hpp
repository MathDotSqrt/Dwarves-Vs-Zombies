#pragma once

#ifndef DVZ_RENDER_SYSTEM_HPP
#define DVZ_RENDER_SYSTEM_HPP

#include "System.hpp"

namespace DVZ::Systems {
	class RenderSystem : public System {
	public:
		RenderSystem();

		void gameTick(Engine& engine) override;
	};
}

#endif