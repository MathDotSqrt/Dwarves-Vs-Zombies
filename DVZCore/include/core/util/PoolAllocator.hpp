#ifndef DVZ_POOL_ALLOCATOR_HPP
#define DVZ_POOL_ALLOCATOR_HPP
#include <memory>
#include <vector>
#include <functional>

namespace DVZ {
	
	template<typename T>
	using AllocatorHandle = std::unique_ptr<T, std::function<void(T*)>>;

	template<typename T>
	class PoolAllocator {
	public:
		static PoolAllocator* getInstance(size_t instances=0) {
			static PoolAllocator allocator(instances);
			return &allocator;
		}

		AllocatorHandle<T> allocate() {
			const auto free_item = [this](T* item) {this->free(item); };

			if (pool.size() > 0) {
				T* item = pool.back();
				pool.pop_back();

				return AllocatorHandle<T>{item, free_item};
			}
			else {
				return AllocatorHandle<T>{new T{}, free_item};
			}
		}

	private:
		PoolAllocator(size_t size) {
			for (size_t i = 0; i < size; i++) {
				pool.push_back(new T{});
			}
		}

		~PoolAllocator() {
			for (T* item : pool) {
				delete item;
			}
		}

		void free(T* handle) {
			pool.push_back(handle);
		}

		std::vector<T*> pool;
	};

}
#endif