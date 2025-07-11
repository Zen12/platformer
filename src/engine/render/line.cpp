#include "line.hpp"


size_t Line::GetVertexCount() const noexcept
{
    return _vertices.size();
}

size_t Line::GetIndicesCount() const noexcept
{
    return _indices.size();
}

void Line::UpdateVertices(const std::vector<float> &vertices) const {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,  static_cast<long>(vertices.size() * sizeof(float)), vertices.data());
}

Line Line::Generate() {
    const std::vector<float> vertices = { 0, 0, 0, 0, 10, 0 }; // Placeholder
    return {vertices};
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
}

Line::~Line() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

void Line::Bind() const noexcept
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_LINES, 0, 2);
}
