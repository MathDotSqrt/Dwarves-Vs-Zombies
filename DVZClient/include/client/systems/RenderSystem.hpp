#pragma once

#ifndef DVZ_RENDER_SYSTEM_HPP
#define DVZ_RENDER_SYSTEM_HPP

#include "client/systems/System.hpp"
#include "client/util/id.hpp"
#include <entt/entity/registry.hpp>
#include <vector>
namespace DVZ::Systems {
	class RenderSystem : public System {
	public:
		RenderSystem();

		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	private:
		void onDeleteInstance(entt::registry& registry, entt::entity entity);
		
		std::vector<DVZ::ID> removeInstanceBuffer;
	};
}

#endif