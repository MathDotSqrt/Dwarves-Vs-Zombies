#pragma once

#ifndef DVZ_GEOMETRY
#define DVZ_GEOMETRY

#include <vector>

#include "core/common.hpp"

namespace DVZ::Graphics {
	template<typename ...T>
	class Geometry {
	private:
		template<typename ...V>
		struct Vertex {};

		template<typename V, typename ...REST>
		struct alignas(4) Vertex<V, REST...> {
			Vertex() {};

			Vertex(
				typename const V::Type& first,
				typename const REST::Type &...rest)
				: first(first), rest(rest...) {}

			typename V::Type first;
			Vertex<REST...> rest;
		};

		template<typename V>
		struct alignas(4) Vertex<V> {
			Vertex() {}
			Vertex(typename const V::Type& last) : last(last) {}

			typename V::Type last;
		};

	public:
		typedef Vertex<T...> GeometryVertex;

		inline void pushVertex(typename const T::Type &...vertexData) {
			verticies.push_back(GeometryVertex(vertexData...));
		}

		inline void pushVertex(const GeometryVertex& vertex) {
			verticies.push_back(vertex);
		}

		inline void pushTriangle(u32 v0, u32 v1, u32 v2) {
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);
		}

		inline void resizeVerticies(size_t newSize) {
			this->verticies.reserve(newSize);
		}

		inline void resizeIndices(size_t newSize) {
			this->indices.reserve(newSize);
		}

		inline void clear() {
			verticies.clear();
			indices.clear();
		}

		inline const std::vector<GeometryVertex>& getVerticies() const {
			return verticies;
		}

		inline const std::vector<unsigned int>& getIndices() const {
			return indices;
		}

		inline void setIndices(const std::vector<u32>& newIndices) {
			indices = newIndices;
		}

		inline size_t getVertexCount() const {
			return verticies.size();
		}

		inline size_t getIndexCount() const {
			return indices.size();
		}

	private:
		std::vector<GeometryVertex> verticies;
		std::vector<u32> indices;
	};
}

#endif