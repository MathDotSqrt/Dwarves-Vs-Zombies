#pragma once 

#ifndef DVZ_PACKED_FREE_LIST_HPP
#define DVZ_PACKED_FREE_LIST_HPP

#include "core/common.hpp"
#include "client/util/id.hpp"
#include <vector>
#include <assert.h>

namespace DVZ::Util {
	template<typename T>
	class PackedFreeList {
	public:

		T& operator[](DVZ::ID id) {
			assert(has(id));
			return packed_array.at(index_array[id.index].index);
		}

		const T& operator[](ID id) const {
			assert(has(id));
			return packed_array.at(index_array[id.index].index);
		}

		T& operator[](u32 index) {
			assert(index < size());
			return packed_array.at(index);
		}

		const T& operator[](u32 index) const {
			assert(index < size());
			return packed_array.at(index);
		}

		DVZ::ID insert(const T& item) {
			T copy = item;
			return insert(std::move(copy));
		}

		DVZ::ID insert(T&& item) {
			u32 packed_index = packed_array.size();

			if (free_length == 0) {
				u32 generation = 0;
				u32 index = index_array.size();
				index_array.push_back({ packed_index, generation });

				packed_array.push_back(std::move(item));
				back_packed_array.push_back(index);
				return { index, generation };
			}
			else {

				DVZ::ID& free_element = index_array[free_index];
				u32 next_index = free_element.index;

				free_element.index = packed_index;
				free_element.generation += 1;

				packed_array.push_back(std::move(item));
				back_packed_array.push_back(free_index);
				free_index = next_index;
				free_length -= 1;

				return { free_index, free_element.generation };
			}
		}

		void remove(DVZ::ID id) {
			assert(has(id));

			u32 element_index = index_array[id.index].index;

			const auto back_element = packed_array.back();
			const auto back_index = back_packed_array.begin();

			index_array[back_index].index = element_index;

			packed_array[element_index] = std::move(back_element);
			back_packed_array[element_index] = back_index;

			packed_array.pop_back();
			back_packed_array.pop_back();

			index_array[id.index].index = free_index;

			free_index = id.index;
			free_length += 1;
		}

		auto begin() {
			return packed_array.begin();
		}

		const auto begin() const {
			return packed_array.begin();
		}

		auto end() {
			return packed_array.end();
		}

		const auto end() const {
			return packed_array.end();
		}

		void clear() {
			index_array.clear();
			packed_array.clear();
			free_length = 0;
		}

		bool has(DVZ::ID id) const {
			if (id == DVZ::NullID)
				return false;
			return index_array[id.index].generation == id.generation;
		}

		size_t size() const {
			return packed_array.size();
		}

	private:
		struct Data {
			T data;
			u32 back_index;
		};

		u32 free_index = 0;
		size_t free_length = 0;

		std::vector<DVZ::ID> index_array;

		std::vector<T> packed_array;
		std::vector<u32> back_packed_array;
	};
}

#endif