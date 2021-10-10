#pragma once
#ifndef DVZ_WINDOW_HPP
#define DVZ_WINDOW_HPP

#include <string>
#include <glm/glm.hpp>

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

		void update();

		bool isPressed(char c) const;
		bool isPressed(Keys keys) const;
		bool isClick(Mouse mouse) const;
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
		static Window* singleton;
		GLFWwindow* window;
		int width;
		int height;
		float scrollDelta = 0;
		bool hasFocus = true;
		bool isMouseDisabled;

		Window(int width, int height, std::string title);

	};
}

#endif