#ifndef DVZ_UTIL_GENERAL_HPP
#define DVZ_UTIL_GENERAL_HPP


#include <unordered_map>

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
}

#endif