#include "core/net/EntitySnapshotDelta.hpp"

using namespace DVZ::Net;



bool EntityStateDelta::hasField(Field field) const {
	if (field == Field::Deleted) {
		return fieldbitset.none();
	}

	return fieldbitset.test(static_cast<std::underlying_type<decltype(field)>::type>(field));
}

void EntityStateDelta::setField(std::initializer_list<Field> fields) {
	for (Field field : fields) {
		fieldbitset.set(static_cast<size_t>(field));
	}
}
