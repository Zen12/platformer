#include "window.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
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

    // need this because of Mac issue
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    _width = fbWidth;
    _height = fbHeight;

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

void Window::PullEvent() const noexcept {
    glfwPollEvents();
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

bool Window::IsFocus() const noexcept {
    return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

void Window::GetMousePosition(double *x, double *y) const noexcept {
    glfwGetCursorPos(window, x, y);
}
