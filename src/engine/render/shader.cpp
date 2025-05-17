#include "shader.h"
#include <GL/glew.h>

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);    

    const char* vertexSourceCstr = vertexSource.c_str();

    glShaderSource(_vertexShader, 1, &vertexSourceCstr, NULL);
    glCompileShader(_vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(_vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    const char* fragmentSourceCstr = fragmentSource.c_str();

    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fragmentShader, 1, &fragmentSourceCstr, NULL);
    glCompileShader(_fragmentShader);

    glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(_fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    _shaderProgram = glCreateProgram();

    glAttachShader(_shaderProgram, _vertexShader);
    glAttachShader(_shaderProgram, _fragmentShader);
    glLinkProgram(_shaderProgram);
}

void Shader::Use() const
{
    glUseProgram(_shaderProgram);
}

const int32_t Shader::GetLocation(const std::string& name) const
{
    return glGetUniformLocation(_shaderProgram, name.c_str());
}

const void Shader::SetVec3(const int32_t& location, const float& x, const float& y, const float& z) const
{
    glUniform3f(location, x, y, z);
}


Shader::~Shader()
{
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader); 
}
