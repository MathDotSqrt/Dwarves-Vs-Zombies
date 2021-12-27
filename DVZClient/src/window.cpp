#include "client/window.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>
using namespace DVZ;

Window* Window::singleton = nullptr;

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

static void DVZ::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        Window::getInstance().hasFocus = false;
    }
}

static void DVZ::internal_focus_callback(GLFWwindow* window, int focused) {
    //if (focused) {
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //    has_focus = true;
    //}
    //else {
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //    has_focus = false;
    //}
}

static void DVZ::internal_mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Window::getInstance().hasFocus = true;
    }
}

static void DVZ::internal_scroll_callback(GLFWwindow* window, double x, double y) {
    Window::getInstance().scrollDelta = (float)y;
}

Window& Window::createInstance(int width, int height, std::string title) {
    assert(Window::singleton == nullptr);

    singleton = new Window(width, height, title);

    return Window::getInstance();
}

Window& Window::getInstance() {
    assert(Window::singleton != nullptr);
    return *Window::singleton;
}

void Window::destroyInstance() {
    glfwDestroyWindow(Window::getInstance().window);
    glfwTerminate();
}

Window::Window(int width, int height, std::string title) : width(width), height(height) {
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(width, height, "OpenGL Boilerplate", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowFocusCallback(window, internal_focus_callback);
    glfwSetMouseButtonCallback(window, internal_mouse_callback);
    glfwSetScrollCallback(window, DVZ::internal_scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
    isMouseDisabled = true;

    glfwSwapInterval(1);

    std::fill(key_pressed.begin(), key_pressed.end(), false);
    std::fill(last_key_pressed.begin(), last_key_pressed.end(), false);

}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::update() {
    scrollDelta = 0;
    last_key_pressed = key_pressed;
    for (char c = 'a'; c <= 'z'; c++) {
        key_pressed[c] = isDown(c);
    }

    last_left_mouse_pressed = left_mouse_pressed;
    last_right_mouse_pressed = right_mouse_pressed;

    left_mouse_pressed = isDown(Mouse::LEFT_CLICK);
    right_mouse_pressed = isDown(Mouse::RIGHT_CLICK);
}

bool Window::isPressed(char c) const {
    return key_pressed[c] && !last_key_pressed[c];
}

bool Window::isDown(char c) const {
    return GLFW_PRESS == glfwGetKey(window, toupper(c));
}

bool Window::isDown(Keys key) const {

    switch (key) {
    case Keys::LEFT_SHIFT:
        return GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    case Keys::LEFT_CTRL:
        return GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    case Keys::ESC:
        return GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE);
    default:
        return false;
    }
}

bool Window::isPressed(Mouse mouse) const {
    switch (mouse) {
    case Mouse::LEFT_CLICK:
        return left_mouse_pressed && !last_left_mouse_pressed;;
    case Mouse::RIGHT_CLICK:
        return right_mouse_pressed && !last_right_mouse_pressed;
    default:
        return false;
    }
}

bool Window::isDown(Mouse mouse) const {
    switch (mouse) {
    case Mouse::LEFT_CLICK:
        return GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    case Mouse::RIGHT_CLICK:
        return GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    default:
        return false;
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

glm::vec2 Window::getMousePos() const {
    static double x, y;
    if (hasFocus)
        glfwGetCursorPos(window, &x, &y);
    return glm::vec2(x, y);
}

float Window::getScrollDelta() const {
    return scrollDelta;
}

int Window::getWidth() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return width;
}

int Window::getHeight() const {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return height;
}