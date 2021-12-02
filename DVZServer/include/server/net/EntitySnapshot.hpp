#ifndef DVZ_ENTITY_SNAPSHOT_HPP
#define DVZ_ENTITY_SNAPSHOT_HPP


#include "core/net/EntitySnapshotDelta.hpp"
#include "core/time.hpp"
#include <unordered_map>
namespace DVZ {
	class Engine;
}

namespace DVZ::Net {
	class EntitySnapshot {
	public:
		EntitySnapshot();
		EntitySnapshot(const Engine& engine);

		static EntitySnapshotDelta computeDelta(const EntitySnapshot& t1, const EntitySnapshot& t2);

		DVZ::simulation_duration getSimulationTime() const;
	private:
		DVZ::simulation_duration simulationTime;
		std::unordered_map<entt::entity, EntityState> entities;
	};
}

#endif 