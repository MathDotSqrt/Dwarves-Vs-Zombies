#pragma once
#ifndef DVZ_VOXEL_SYSTEM_HPP
#define DVZ_VOXEL_SYSTEM_HPP

#include "client/systems/System.hpp"


namespace DVZ::Systems {
	class VoxelSystem : public System {
	public:
		void init(Engine& engine) override;
		void gameTick(Engine& engine) override;
	};
}

#endif