#pragma once
#ifndef DVZ_UTIL_HPP
#define DVZ_UTIL_HPP

#include <glm/glm.hpp>

#include <assert.h>

namespace DVZ::Util {

	template<typename T, typename FUNC>
	void iterate_volume(const glm::vec<3, T>& min, const glm::vec<3, T>& max, FUNC&& func ) {
		assert(min.x <= max.x);
		assert(min.y <= max.y);
		assert(min.z <= max.z);

		//TODO this may overflow
		for (T y = min.y; y <= max.y; y++) {
			for (T z = min.z; z <= max.z; z++) {
				for (T x = min.x; x <= max.x; x++) {
					func(glm::vec<3, T>(x, y, z));
				}
			}
		}
	}
}

#endif 