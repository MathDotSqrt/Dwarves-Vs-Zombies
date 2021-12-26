#ifndef DVZ_MATH_HPP
#define DVZ_MATH_HPP

#include <glm/glm.hpp>

namespace DVZ {
	template<typename T>
	auto compMaxIndex(const glm::vec<3, T> v) {
		return v.y > v.x ? (v.z > v.y ? 2 : 1) : (v.z > v.x ? 2 : 0);
	}

	template<typename T>
	auto compMinIndex(const glm::vec<3, T> v) {
		return v.y < v.x ? (v.z < v.y ? 2 : 1) : (v.z < v.x ? 2 : 0);
	}
}

#endif