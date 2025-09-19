#include "window.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

GLFWwindow *window;

// c-like callback from GLFW for input
void key_callback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]]  int mods)
{
    if (auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window))) {
        self->OnKeyCode(key, action);
    }
}

// c-like callback from GLFW to resize window
void framebuffer_size_callback(GLFWwindow *window, const int width, const int height)
{
    glViewport(0, 0, width, height);
    if (auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window)))
        self->OnResize(width, height);
}

void mouse_button_callback(GLFWwindow * window, int button, int action, [[maybe_unused]]  int mods) {
    if (auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window))) {
        self->OnMouse(button, action);
    }
}

Window::Window(const uint16_t &width, const uint16_t &height, const std::string &windowName)
{
    _width = width;
    _height = height;

    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(_width, _height, windowName.c_str(), nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to init GLEW\n";
        return;
    }

#ifdef __EMSCRIPTEN__
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    _width = static_cast<uint16_t>(fbWidth);
    _height = static_cast<uint16_t>(fbHeight);
#endif



#ifdef __APPLE__
    // need this because of Mac retina thing
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    if (fbWidth != _width || fbHeight != _height) {
        glfwSetWindowSize(window, _width / 2, _height / 2);
    }
#endif
    // input
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void Window::WinInit() {
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

bool Window::IsOpen() const noexcept {
    return window && !glfwWindowShouldClose(window);
}

void Window::Destroy() const noexcept {
    glfwDestroyWindow(window);
    glfwTerminate();
    window = nullptr;
}

void Window::SwapBuffers() const noexcept {
    glfwSwapBuffers(window);
}

void Window::PullEvent() noexcept {

    // emscripten doesn't control the poll events, so they are been cached
#ifdef __EMSCRIPTEN__
    for (const auto &[key, value] : _keyboardCodesCached) {
        _keyboardCodes[key] = value;
    }
    _keyboardCodesCached.clear();

    for (const auto &[key, value] : _mouseCodesCached) {
        _mouseCodes[key] = value;
    }
    _mouseCodesCached.clear();

#else
    glfwPollEvents();
#endif
}

void Window::ClearWithValue(const int &value) noexcept {

    for (auto it = _keyboardCodes.begin(); it != _keyboardCodes.end(); ) {
        if (it->second == value) {
            it = _keyboardCodes.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = _mouseCodes.begin(); it != _mouseCodes.end(); ) {
        if (it->second == value) {
            it = _mouseCodes.erase(it);
        } else {
            ++it;
        }
    }
}

void Window::SwapWithValue(const int &target, const int &newValue) noexcept {

    for (auto it = _keyboardCodes.begin(); it != _keyboardCodes.end(); ) {
        if (it->second == target) {
            it->second = newValue;
        } else {
            ++it;
        }
    }

    for (auto it = _mouseCodes.begin(); it != _mouseCodes.end(); ) {
        if (it->second == target) {
            it->second = newValue;
        } else {
            ++it;
        }
    }
}

bool Window::IsFocus() const noexcept {
    return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

void Window::GetMousePosition(double *x, double *y) const noexcept {
    glfwGetCursorPos(window, x, y);
    // find a better way to handle physical and logical pixels for emscripten to not adapt mouse position to it
#ifdef __EMSCRIPTEN__
    *x /= 2.0;
    *y /= 2.0;
#endif
}
