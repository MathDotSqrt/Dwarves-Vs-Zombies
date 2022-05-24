#ifndef DVZ_UTIL_GENERAL_HPP
#define DVZ_UTIL_GENERAL_HPP

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace DVZ {
	template<typename KEY, typename VALUE, typename FUNC>
	void remove_if(std::unordered_map<KEY, VALUE>& map, FUNC& pred) {
		auto iter = map.begin();
		const auto end = map.end();
		while (iter != end) {
			if (pred(iter->first, iter->second)) {
				iter = map.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	template<typename KEY, typename VALUE>
	bool contains(const std::unordered_map<KEY, VALUE>& map, const KEY& key) {
		const auto iter = map.find(key);
		return iter != map.end();
	}
}

#endif