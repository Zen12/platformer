#include "mesh.hpp"
#include <GL/glew.h>

#define DEBUG_ENGINE_MESH 0

Mesh::Mesh(
    const std::vector<float> &vertices,
    const std::vector<uint32_t> &indices)
{
    _vertices = vertices;
    _indices = indices;

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, (long)(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);

    int32_t stride = (int32_t)(5 * sizeof(float));
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
#ifndef NDEBUG
#if DEBUG_ENGINE_MESH
    std::cout << "Mesh Destroyed " << std::endl;
    std::cout << "VAO: " << _vao << " VBO: " << _vbo << std::endl;
    std::cout << "\n";
#endif
#endif
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void Mesh::Bind() const noexcept
{
    glBindVertexArray(_vao);
}

size_t Mesh::GetVertexCount() const noexcept
{
    return _vertices.size();
}

size_t Mesh::GetIndicesCount() const noexcept
{
    return _indices.size();
}

Mesh Mesh::GenerateSprite() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const std::vector<float> vertices = {
        // positions                // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    };
    const std::vector<uint32_t> indices = {
        0, 3, 1,
        1, 3, 2};

    return {vertices, indices};
}

Mesh Mesh::GenerateUI()
{
    const std::vector<float> vertices = {
        // positions                // texture coords
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    const std::vector<uint32_t> indices = {
        0, 3, 1,
        1, 3, 2};

    return {vertices, indices};
}
