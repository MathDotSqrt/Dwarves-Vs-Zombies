#ifndef DVZ_PHYSICS_SYSTEM_HPP
#define DVZ_PHYSICS_SYSTEM_HPP

namespace DVZ {
	class Engine;

	class PhysicsSystem {
	public:
		PhysicsSystem();
		void init(Engine& engine);
		void tick(Engine& engine);
	};
}

#endif 