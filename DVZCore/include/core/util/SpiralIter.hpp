#include <iterator>
#include <glm/glm.hpp>
#include <assert.h>

namespace DVZ {

	template<typename IndexType>
	class SpiralIter {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = glm::vec<2, IndexType>;
		using pointer = const value_type*;
		using reference = const value_type&;


		SpiralIter() {}

		SpiralIter(value_type center, IndexType radius) {
			if (radius > 0) {
				this->current_coord = center;
				this->radius = radius;
				this->layer = 1;
				this->leg = 0;
			}
		}

		value_type operator*() const { return current_coord; }
		pointer operator->() const { return &current_coord; }

		// Prefix increment
		SpiralIter& operator++() {
			assert((*this) != SpiralIter{});

			
			if (current_coord.x == radius && current_coord.y == -radius) {
				(*this) = SpiralIter{};
			}
			else {
				auto& x = current_coord.x;
				auto& y = current_coord.y;
				switch (leg) {
				case 0:
					++x; if (x == layer) ++leg; break;
				case 1:
					++y; if (y == layer) ++leg; break;
				case 2:
					--x; if (-x == layer) ++leg; break;
				case 3:
					--y; if (-y == layer) { leg = 0; ++layer; } break;
				}
			}

			return *this;
		}

		// Postfix increment
		SpiralIter operator++(int) { SpiralIter tmp = *this; ++(*this); return tmp; }

		bool operator== (const SpiralIter& other) const { 
			return other.current_coord == current_coord 
				&& other.layer == layer
				&& other.leg == leg;
		};
		
		bool operator!= (const SpiralIter& other) const { 
			return !((*this) == other);
		};

	private:
		value_type current_coord = value_type{0};
		IndexType layer = 0;
		IndexType leg = 0;
		IndexType radius = -1;
	};


	template<typename IndexType>
	class SpiralRange {
	public:
		SpiralRange(typename SpiralIter<IndexType>::value_type center, IndexType radius) : iter(center, radius) {
			
		}

		SpiralIter<IndexType> begin() const { return iter; };
		SpiralIter<IndexType> end() const { return SpiralIter<IndexType>{}; };
	private:
		SpiralIter<IndexType> iter;
	};
}