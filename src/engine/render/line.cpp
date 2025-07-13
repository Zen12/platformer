#include "line.hpp"

#define DEBUG_ENGINE_LINES 0

#ifndef NDEBUG
#if DEBUG_ENGINE_LINES
#include <iostream>
#endif
#endif


size_t Line::GetVertexCount() const noexcept
{
    return _vertices.size();
}

size_t Line::GetIndicesCount() const noexcept
{
    return _indices.size();
}

void Line::UpdateVertices(const std::vector<float> &vertices) {

#ifndef NDEBUG
#if DEBUG_ENGINE_LINES
    std::cout << "Update vertices " << std::endl;
    std::cout << "VAO: " << _vao << " VBO: " << _vbo << std::endl;
    std::cout << "Size: " << vertices.size() << std::endl;
    for (size_t i = 0; i < vertices.size(); i++) {
        std::cout << vertices[i] << " ";
    }
    std::cout << "\n";
#endif
#endif

    _vertices = vertices;

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,  static_cast<long>(vertices.size() * sizeof(float)), vertices.data());
}

Line Line::Generate() {
    const std::vector<float> vertices = { 0, 0, 0, 0, 10, 0 }; // Placeholder
    return Line(vertices);
}

Line::Line(const std::vector<float> &vertices) {
    _vertices = vertices;

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER,  static_cast<long>(_vertices.size() * sizeof(float)), _vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

#ifndef NDEBUG
#if DEBUG_ENGINE_LINES
    std::cout << "\n";
    std::cout << "Created Line " << std::endl;
    std::cout << "VAO: " << _vao << " VBO: " << _vbo << std::endl;
    std::cout << "Size: " << vertices.size() << std::endl;
    for (size_t i = 0; i < vertices.size(); i++) {
        std::cout << vertices[i] << " ";
    }
    std::cout << "\n";
#endif
#endif
}

Line::~Line() {
#ifndef NDEBUG
#if DEBUG_ENGINE_LINES
    std::cout << "Destroyed Line " << std::endl;
    std::cout << "VAO: " << _vao << " VBO: " << _vbo << std::endl;
    std::cout << "\n";
#endif
#endif
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

void Line::Bind() const noexcept
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_LINES, 0, 2);
}
