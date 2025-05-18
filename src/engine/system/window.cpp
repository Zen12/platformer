#include "window.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


GLFWwindow* window;

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) 
        self->OnResize(width, height);
}


Window::Window(const uint16_t &width, const uint16_t &height, const std::string &windowName)
{
    _width = width;
    _height = height;

    if (!glfwInit()) {
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
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW\n";
        return;
    }

    // need this mecause of Mac issue
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    _width = fbWidth;
    _height = fbHeight;
}

void Window::WinInit()
{
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

bool Window::IsOpen()
{
    return !glfwWindowShouldClose(window);
}


void Window::Destroy()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(window);
}
