
#include "core/util/Timer.hpp"
#include "core/common.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <thread>


using namespace DVZ;

Timer::Timer(std::string_view name) : m_name(name) {
	m_start_time = std::chrono::high_resolution_clock::now();

	MasterTimer::getInstance()->pushTimer(this);
}

Timer::~Timer() {
	m_end_time = std::chrono::high_resolution_clock::now();

	MasterTimer::getInstance()->popTimer(this);
}

Timer::duration_type Timer::getDelta() const {
	return m_end_time - m_start_time;
}

std::string_view Timer::getName() const {
	return m_name;
}

RootTimer::RootTimer(std::string_view name) : m_name(name) {
	std::stringstream ss;
	ss << std::this_thread::get_id();
	m_thread_id = ss.get();

	m_timer = new Timer{"Root"};
}

RootTimer::~RootTimer() {
	delete m_timer;

	MasterTimer::getInstance()->clearTimer();

	if (m_enabled_print) {
		spdlog::info("{}: [{}]\n{}", m_name, m_thread_id, MasterTimer::getInstance()->printTimer());
	}
}

void RootTimer::enablePrint() {
	m_enabled_print = true;
}

MasterTimer::Node::Node(std::string_view name) : name(name), duration(0), duration_buffer(20) {
} 

MasterTimer::MasterTimer() {
	m_root = std::make_unique<Node>(std::string_view{ "" });
	m_timer_stack.push_back(m_root.get());
}

MasterTimer* MasterTimer::getInstance() {
	thread_local MasterTimer master;
	return &master;
}

std::string MasterTimer::printTimer() const {
	std::string out;
	

	const auto root = m_timer_stack.back();
	std::vector<std::pair<const Node*, int>> stack;
	for (const auto& value : root->children) {
		stack.push_back(std::make_pair(value.get(), 0));
	}


	while (stack.size()) {
		const auto [current_ptr, depth] = stack.back();
		stack.pop_back();

		const auto [min, max, avg] = computeStatistics(current_ptr);
		out += fmt::format("{:\t>{}}{}: {:.2f}ms [{:.2f}, {:.2f}]ms\n", "", depth, current_ptr->name, avg, min, max);

		size_t current_index = stack.size();
		for (const auto& value : current_ptr->children) {
			stack.push_back(std::make_pair(value.get(), depth + 1));
		}
		std::reverse(stack.begin() + current_index, stack.end());
	}
	return out;
}

void MasterTimer::clearTimer() {
	std::vector<Node*> stack = { m_timer_stack.back() };

	while (stack.size()) {
		Node* current_ptr = stack.back();
		stack.pop_back();

		//current_ptr->duration_buffer.insert(current_ptr->duration);
		current_ptr->duration_buffer.insert(current_ptr->duration);
		current_ptr->duration = Timer::duration_type{ 0 };

		for (const auto& value : current_ptr->children) {
			stack.push_back(value.get());
		}
	}
}

void MasterTimer::pushTimer(const Timer* timer) {
	const auto is_timer = [&](const auto& value) {
		return value->name == timer->getName();
	};

	auto& children = m_timer_stack.back()->children;
	const auto iter = std::find_if(children.begin(), children.end(), is_timer);

	if (iter != children.end()) {
		m_timer_stack.push_back(iter->get());
	}
	else {
		Node* ptr = children.emplace_back(std::make_unique<Node>(timer->getName())).get();
		m_timer_stack.push_back(ptr);
	}
}

void MasterTimer::popTimer(const Timer* timer){
	m_timer_stack.back()->duration += timer->getDelta();
	m_timer_stack.pop_back();
}

std::tuple<double, double, double> MasterTimer::computeStatistics(const Node* node) const {
	Timer::duration_type min = node->duration_buffer.front();
	Timer::duration_type max = node->duration_buffer.front();
	Timer::duration_type total = Timer::duration_type{0};

	for (const auto& value : node->duration_buffer) {
		if (value < min) {
			min = value;
		}
		if (max < value) {
			max = value;
		}
		total += value;
	}

	const auto avg = total / node->duration_buffer.size();
	return std::make_tuple(min.count(), max.count(), avg.count());
}