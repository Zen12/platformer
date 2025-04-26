#include "window.h"
#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


GLFWwindow* window;

Window::Window(const uint16_t &width, const uint16_t &height, const std::string &windowName)
{
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
