#pragma once
#ifndef DVZ_WINDOW_HPP
#define DVZ_WINDOW_HPP

#include <string>
#include <glm/glm.hpp>
#include <array>
#include <bitset>
#include <mutex>
#include <vector>

struct GLFWwindow;


namespace DVZ {

	//free functions for glfw's c style callbacks
	static void key_callback(GLFWwindow*, int, int, int, int);
	static void internal_focus_callback(GLFWwindow*, int);
	static void internal_mouse_callback(GLFWwindow*, int, int, int);
	static void internal_scroll_callback(GLFWwindow*, double, double);

	class Window {
	public:
		enum class Keys {
			LEFT_SHIFT,
			LEFT_CTRL,
			ESC
		};

		enum class Mouse {
			RIGHT_CLICK,
			LEFT_CLICK
		};


		static Window& createInstance(int width, int height, std::string title);
		static Window& getInstance();
		static void destroyInstance();

		void pollWindow();
		void update();

		bool isPressed(char c) const;
		bool isPressed(Mouse mouse) const;


		bool isDown(char c) const;
		bool isDown(Keys keys) const;
		bool isDown(Mouse mouse) const;
		bool shouldClose() const;

		glm::vec2 getMousePos() const;
		float getScrollDelta() const;

		int getWidth() const;
		int getHeight() const;

		//allow access to window private variables
		friend void key_callback(GLFWwindow*, int, int, int, int);
		friend void internal_focus_callback(GLFWwindow*, int);
		friend void internal_mouse_callback(GLFWwindow*, int, int, int);
		friend void internal_scroll_callback(GLFWwindow*, double, double);

	private:

		struct WindowInputState{
			std::bitset<127> pressed_keys;
			std::bitset<127> down_keys;
			bool pressed_left_mouse = false;
			bool down_left_mouse = false;
			bool pressed_right_mouse = false;
			bool down_right_mouse = false;
		};

		static Window* singleton;
		GLFWwindow* window;
		int width;
		int height;
		float scrollDelta = 0;
		bool hasFocus = true;
		bool isMouseDisabled;
		WindowInputState gameloop_input_state;

		WindowInputState prev_renderloop_input_state;
		WindowInputState renderloop_input_state;
		std::vector<WindowInputState> input_buffer;
		std::mutex input_buffer_mutex;
		std::thread::id render_thread_id = std::this_thread::get_id();

		Window(int width, int height, std::string title);

	};
}

#endif
