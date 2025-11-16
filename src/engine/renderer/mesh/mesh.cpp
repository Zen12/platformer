#include "mesh.hpp"
#include <GL/glew.h>

#define DEBUG_ENGINE_MESH 0

Mesh::Mesh(
    const std::vector<float> &vertices,
    const std::vector<uint32_t> &indices,
    const bool &useTexture)
{
    _verticesCount = vertices.size();
    _indicesCount = indices.size();

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(float)), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(uint32_t)), indices.data(), GL_DYNAMIC_DRAW);

    int32_t stride = useTexture ? static_cast<int32_t>(5 * sizeof(float)) : static_cast<int32_t>(3 * sizeof(float));


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);
    if (useTexture) {
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh(const Rml::Span<const Rml::Vertex> &vertices, const Rml::Span<const int> &indices) {

    _verticesCount = vertices.size();
    _indicesCount = indices.size();
    // Create VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Create and upload VBO
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Rml::Vertex), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, position));

    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, colour));

    // Tex coord attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, tex_coord));

    // Create and upload EBO
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::UpdateData(const std::vector<float> &vertices, const std::vector<uint32_t> &indices) noexcept {

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (vertices.size() <= _verticesCount) {
        glBufferSubData(GL_ARRAY_BUFFER, 0,  static_cast<long>(vertices.size() * sizeof(float)), vertices.data());
    } else {
        glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(float)), vertices.data(), GL_DYNAMIC_DRAW);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    if (indices.size() <= _indicesCount) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<long>(indices.size() * sizeof(uint32_t)), indices.data());
    } else {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(uint32_t)), indices.data(), GL_DYNAMIC_DRAW);
    }


    _verticesCount = vertices.size();
    _indicesCount = indices.size();
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
    return _verticesCount;
}

size_t Mesh::GetIndicesCount() const noexcept
{
    return _indicesCount;
}

Mesh Mesh::GenerateSquare() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const std::vector<float> vertices = {
        // positions                // texture coords
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
    };
    const std::vector<uint32_t> indices = {
        0, 3, 1,
        1, 3, 2};

    return {vertices, indices, false};
}

std::unique_ptr<Mesh> Mesh::GenerateSpritePtr() {
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

    return std::make_unique<Mesh>(vertices, indices, true);
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

    return {vertices, indices, true};
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

    return {vertices, indices, true};
}
