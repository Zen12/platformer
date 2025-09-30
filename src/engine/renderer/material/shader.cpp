#include "shader.hpp"
#include <GL/glew.h>

Shader::Shader(const std::string &vertexSource, const std::string &fragmentSource)
{
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *sourceArray[] = {vertexSource.c_str()}; // stack-alloc array, RAII delete

    glShaderSource(_vertexShader, 1, sourceArray, NULL);
    glCompileShader(_vertexShader);

    int success;
    glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(_vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << vertexSource
                  << infoLog << std::endl;
    }

    const GLchar *fragmentArray[] = {fragmentSource.c_str()}; // stack-alloc array, RAII delete

    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fragmentShader, 1, fragmentArray, NULL);
    glCompileShader(_fragmentShader);

    glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(_fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << fragmentSource
                  << infoLog << std::endl;
    }

    _shaderProgram = glCreateProgram();

    glAttachShader(_shaderProgram, _vertexShader);
    glAttachShader(_shaderProgram, _fragmentShader);
    glLinkProgram(_shaderProgram);
}

void Shader::Use() const noexcept
{
    glUseProgram(_shaderProgram);
}

int32_t Shader::GetLocation(const std::string &name) const noexcept
{
    return glGetUniformLocation(_shaderProgram, name.c_str());
}

void Shader::SetVec4(const int32_t &location, const float &x, const float &y, const float &z, const float &w) const noexcept
{
    glUniform4f(location, x, y, z, w);
}

void Shader::SetVec3(const int32_t &location, const float &x, const float &y, const float &z) const noexcept
{
    glUniform3f(location, x, y, z);
}

void Shader::SetVec2(const int32_t &location, const float &x, const float &y) const noexcept
{
    glUniform2f(location, x, y);
}

void Shader::SetInt(const int32_t &location, const int &x) const noexcept
{
    glUniform1i(location, x);
}

void Shader::SetFloat(const int32_t &location, const float &x) const noexcept
{
    glUniform1f(location, x);
}

void Shader::SetMat4(const int32_t &location, const glm::mat4 &mat) const noexcept
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

int32_t Shader::GetShaderId() const noexcept
{
    return _shaderProgram;
}

Shader::~Shader()
{
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
}
