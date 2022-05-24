#ifndef DVZ_SERVER_VOXEL_SYSTEM_HPP
#define DVZ_SERVER_VOXEL_SYSTEM_HPP

namespace DVZ {
	class Engine;

	class ServerVoxelSystem {
	public:
		void init(Engine& engine);
		void tick(Engine& engine);
	};
}

#endif 