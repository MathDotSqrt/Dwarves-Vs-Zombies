#ifndef DVZ_ENTITY_SNAPSHOT_DELTA_HPP
#define DVZ_ENTITY_SNAPSHOT_DELTA_HPP

#include "core/common.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <bitset>

namespace DVZ::Net {

	struct EntityState {
		glm::vec3 pos{};
		glm::quat rot{};
	};

	class EntityStateDelta {
	public:
		enum class Field : u8 {
			Deleted = 0,
			Spawned,
			Position,
			Rot,
			NUM_FIELDS
		};

		entt::entity entity;
		EntityState state;
		std::bitset<static_cast<size_t>(Field::NUM_FIELDS)> fieldbitset;

		bool hasField(Field field) const;
		void setField(std::initializer_list<Field> fields);
		void setField(Field field);

		template <class Archive>
		void serialize(Archive& ar) {
			ar(entity, fieldbitset);
			if (hasField(Field::Position)) {
				ar(state.pos.x, state.pos.y, state.pos.z);
			}
			if (hasField(Field::Rot)) {
				ar(state.rot.x, state.rot.y, state.rot.z, state.rot.w);
			}
		}
	};

	using EntitySnapshotDelta = std::vector<EntityStateDelta>;
}

#endif