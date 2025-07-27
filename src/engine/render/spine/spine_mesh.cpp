#include "spine_mesh.hpp"
#include <GL/glew.h>

#define DEBUG_ENGINE_SPINE 0

SpineMesh::SpineMesh(
    const std::vector<float> &vertices,
    const std::vector<uint32_t> &indices,
    const bool &useTexture)
{
    _vertices = vertices;
    _indices = indices;

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, (long)(vertices.size() * sizeof(float)), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (long)(indices.size() * sizeof(uint32_t)), indices.data(), GL_DYNAMIC_DRAW);

    int32_t stride = useTexture ? (int32_t)(5 * sizeof(float)) : (int32_t)(3 * sizeof(float));


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);
    if (useTexture) {
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}


SpineMesh::~SpineMesh()
{
#ifndef NDEBUG
#if DEBUG_ENGINE_SPINE
    std::cout << "Spine Destroyed " << std::endl;
    std::cout << "VAO: " << _vao << " VBO: " << _vbo << std::endl;
    std::cout << "\n";
#endif
#endif
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void SpineMesh::Bind() const noexcept
{
    glBindVertexArray(_vao);
}

size_t SpineMesh::GetVertexCount() const noexcept
{
    return _vertices.size();
}

size_t SpineMesh::GetIndicesCount() const noexcept
{
    return _indices.size();
}

SpineMesh SpineMesh::Create() {
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

    return {vertices, indices, true};
}
