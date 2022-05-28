
#include <vector>

namespace DVZ {
	
	template<typename T>
	class CircularBuffer {
	public:
		CircularBuffer(size_t max_size) : m_max_size(max_size) {
			m_data.reserve(max_size);
		}

		void insert(const T& value) {
			if (m_data.size() == m_max_size) {
				m_data[m_current_index] = value;
				++m_current_index;
				if (m_current_index == m_max_size) {
					m_current_index = 0;
				}
			}
			else {
				m_data.push_back(value);
			}
		}

		auto begin() {
			return m_data.begin();
		}

		const auto begin() const {
			return m_data.cbegin();
		}

		auto end() {
			return m_data.end();
		}

		const auto end() const {
			return m_data.cend();
		}

		const auto& front() const {
			return m_data.front();
		}

		const auto& back() const {
			return m_data.back();
		}

		size_t size() const {
			return m_data.size();
		}

	private:
		std::vector<T> m_data;
		size_t m_max_size;
		size_t m_current_index = 0;
	};
}