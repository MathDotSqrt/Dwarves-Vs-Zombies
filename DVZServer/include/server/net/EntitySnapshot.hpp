#ifndef DVZ_ENTITY_SNAPSHOT_HPP
#define DVZ_ENTITY_SNAPSHOT_HPP


#include "core/net/EntitySnapshotDelta.hpp"
#include <map>
namespace DVZ {
	class Engine;
}

namespace DVZ::Net {
	class EntitySnapshot {
	public:
		EntitySnapshot(const Engine& engine);

	private:
		std::map<entt::entity, EntityState> entities;
	};

	EntitySnapshotDelta computeDelta(const EntitySnapshot& t1, const EntitySnapshot& t2);
}

#endif 