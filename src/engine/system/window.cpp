#include "window.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

GLFWwindow *window;

// c-like callback from GLFW for input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// c-like callback from GLFW to resize window
void framebuffer_size_callback(GLFWwindow *window, const int width, const int height)
{
    glViewport(0, 0, width, height);
    if (auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window)))
        self->OnResize(width, height);
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
}

void Window::WinInit()
{
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

bool Window::IsOpen() const
{
    return !glfwWindowShouldClose(window);
}

void Window::Destroy() const
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(window);
}
