#include "server/net/EntitySnapshot.hpp"
#include "server/Engine.hpp"
#include "server/ServerComponents.hpp"
#include "core/CoreComponents.hpp"

using namespace DVZ::Net;

EntitySnapshot::EntitySnapshot() {
	simulationTime = DVZ::simulation_duration{ 0 };
}

EntitySnapshot::EntitySnapshot(const Engine& engine) {
	const auto& registry = engine.getRegistry();
	
	auto view = registry.view<const Network, const Transformation>();

	for (entt::entity entity : view) {
		const auto& transform = view.get<const Transformation>(entity);
		EntityState state{transform.pos, transform.rot};
		
		entities[entity] = state;
	}

	simulationTime = engine.getTimeElapsed();
}

DVZ::simulation_duration EntitySnapshot::getSimulationTime() const{
	return simulationTime;
}

EntitySnapshotDelta EntitySnapshot::computeDelta(const EntitySnapshot& t1, const EntitySnapshot& t2) {
	
	EntitySnapshotDelta snapshotDelta;

	for (const auto& [t2_entity, t2_state] : t2.entities) {
		const auto iter = t1.entities.find(t2_entity);
		if (iter != t1.entities.end()) {
			bool update_entity = false;
			EntityStateDelta delta;
			const auto& [t1_entity, t1_state] = *iter;

			if (t1_state.pos != t2_state.pos) {
				delta.setField(EntityStateDelta::Field::Position);
				delta.state.pos = t2_state.pos;
				update_entity = true;
			}
			if (t1_state.rot != t2_state.rot) {
				delta.setField(EntityStateDelta::Field::Rot);
				delta.state.rot = t2_state.rot;
				update_entity = true;
			}

			if (update_entity) {
				snapshotDelta.push_back(delta);
			}
		}
		else {
			EntityStateDelta& delta = snapshotDelta.emplace_back();

			delta.setField({ EntityStateDelta::Field::Spawned, EntityStateDelta::Field::Position, EntityStateDelta::Field::Rot});
			delta.state = t2_state;
		}
	}

	for (const auto& [t1_entity, t1_state] : t1.entities) {
		const auto iter = t2.entities.find(t1_entity);
		if (iter == t2.entities.end()) {
			EntityStateDelta& delta = snapshotDelta.emplace_back();
			delta.setField(EntityStateDelta::Field::Deleted);
		}
	}

	return snapshotDelta;
}