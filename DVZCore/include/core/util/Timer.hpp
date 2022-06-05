
#include "core/util/CircularBuffer.hpp"

#include <string_view>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <string>

namespace DVZ {

	using timer_time_point_t = std::chrono::high_resolution_clock::time_point;

	class Timer {
	public:
		using duration_type = std::chrono::duration<double, std::milli>;

		Timer(std::string_view name);
		~Timer();

		std::string_view getName() const;
		duration_type getDelta() const;
	private:
		

		std::string_view m_name;
		timer_time_point_t m_start_time;
		timer_time_point_t m_end_time;

	};

	class RootTimer {
	public:
		RootTimer(std::string_view name);
		~RootTimer();

		void enablePrint();
		void enablePrint(Timer::duration_type max_thresh);

	private:
		std::string_view m_name;
		std::string m_thread_id;
		Timer* m_timer = nullptr;
		bool m_enabled_print = false;
		Timer::duration_type m_max_thresh = Timer::duration_type{0};
	};

	class MasterTimer {
	public:
		static MasterTimer* getInstance();

		std::string printTimer() const;
		void clearTimer();

		Timer::duration_type getMaxTimer() const;
	private:
		friend class Timer;

		struct Node {
			Node(std::string_view name);
			std::string_view name;
			Timer::duration_type duration;

			CircularBuffer<Timer::duration_type> duration_buffer;
			std::vector<std::unique_ptr<Node>> children;
			//std::unordered_map<std::string_view, std::unique_ptr<Node>> children;
		};

		MasterTimer();

		void pushTimer(const Timer* timer);
		void popTimer(const Timer* timer);

		std::tuple<double, double, double> computeStatistics(const Node* node) const;

		std::unique_ptr<Node> m_root;
		std::vector<Node*> m_timer_stack;
		Timer::duration_type m_max_duration = Timer::duration_type{0};
	};
}