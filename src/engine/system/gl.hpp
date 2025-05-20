#include <functional>
#include <cassert>
#include <iostream>
#include <GL/glew.h>

class GL
{
public:
    static void CALL(const std::function<void()> &callback)
    {
        callback();
#ifndef NDEBUG
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error: " << err << "\n";
        }
#endif
    }
};