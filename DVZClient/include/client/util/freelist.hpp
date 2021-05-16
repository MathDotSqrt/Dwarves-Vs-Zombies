#pragma once 

#ifndef DVZ_FREELIST_HPP
#define DVZ_FREELIST_HPP

#include "client/util/id.hpp"

#include <vector>

namespace DVZ::Util {
	
	template<typename T>
	class freelist {
	public:

		T& operator[](DVZ::ID id) {
			assert(has(id));
			return v.at(id.index).elem;
		}

		const T& operator[](ID id) const {
			assert(has(id));
			return v.at(id.index).elem;
		}

		T& operator[](u32 index) {
			assert(index < size());
			return v.at(index).elem;
		}

		const T& operator[](u32 index) const {
			assert(index < size());
			return v.at(index).elem;
		}

		u32 get_generation(u32 index) {
			return v[index].id.generation;
		}

		DVZ::ID push_back(const T& item) {
			T copy = item;
			return insert(std::move(copy));
		}

		DVZ::ID push_back(T&& item) {
			if (free_length == 0) {
				v.emplace_back(Node{ std::move(item), ID{INDEX_SENTINEL, 0} });
				return DVZ::ID{(u32)v.size() - 1, 0};
			}

			u32 new_index = free_index;

			v[free_index].elem = std::move(item);
			ID& new_id = v[free_index].id;

			free_index = new_id.index;
			free_length -= 1;

			new_id.index = INDEX_SENTINEL;
			return new_id;
		}

		auto begin() {
			return iterator(v.data(), v.data() + v.size());
		}

		const auto begin() const {
			return iterator(v.data(), v.data() + v.size());
		}

		auto end() {
			return iterator(v.data() + v.size(), v.data() + v.size());
		}

		const auto end() const {
			return iterator(v.data() + v.size(), v.data() + v.size());
		}

		void remove(DVZ::ID id) {
			assert(has(id));

			v[id.index].elem.~T();
			v[id.index].id.generation += 1;
			v[id.index].id.index = free_index;
			free_index = id.index;
		}

		void clear() {
			free_index = 0;
			free_length = 0;
			v.clear();
		}

		bool has(DVZ::ID id) const {
			if (id == DVZ::NullID)
				return false;
			if (id.index >= v.size())
				return false;

			return v[id.index].id.generation == id.generation;
		}

		bool has(u32 index) const {
			return v[index].id.index == INDEX_SENTINEL;
		}

		size_t size() const {
			return v.size();
		}

		struct Node {
			T elem;
			DVZ::ID id;
		};

		struct iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			iterator(Node* ptr, Node* end) : ptr(ptr), end(end){

			}

			reference operator*() { return ptr->elem; }
			pointer operator->() { return &ptr->elem; }
			iterator& operator++() {
				++ptr;
				while (ptr != end && ptr->id.index != INDEX_SENTINEL) {
					++ptr;
				}
				return *this; 
			}
			iterator operator++(int) { 
				iterator tmp = *this;
				++(*this);
				return tmp;
			}
			friend bool operator== (const iterator& a, const iterator& b) { return a.ptr == b.ptr; };
			friend bool operator!= (const iterator& a, const iterator& b) { return a.ptr != b.ptr; };
		private:
			Node* ptr;
			Node* end;
		};
	private:
		static constexpr u32 INDEX_SENTINEL = -1;

		u32 free_index = 0;
		size_t free_length = 0;

		std::vector<DVZ::Util::freelist<T>::Node> v;

	};
}

#endif