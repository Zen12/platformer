#include "mesh.hpp"
#include <GL/glew.h>

#define DEBUG_ENGINE_MESH 0

#if DEBUG_ENGINE_MESH
#include <iostream>
#define MESH_LOG if(0) std::cout
#else
#define MESH_LOG if(0) std::cout
#endif

Mesh::Mesh(
    const std::vector<float> &vertices,
    const std::vector<uint32_t> &indices,
    const bool &useTexture)
{
    MESH_LOG << "[MESH] Creating mesh - vertices: " << vertices.size() << ", indices: " << indices.size()
              << ", useTexture: " << (useTexture ? "yes" : "no") << std::endl;
    _verticesCount = vertices.size();
    _indicesCount = indices.size();

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    MESH_LOG << "[MESH] Generated OpenGL buffers - VAO: " << _vao << ", VBO: " << _vbo << ", EBO: " << _ebo << std::endl;
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

Mesh::Mesh(
    const std::vector<float> &vertices,
    const std::vector<uint32_t> &indices,
    const bool &useTexture,
    const std::vector<float> &boneWeights,
    const std::vector<int> &boneIndices)
{
    MESH_LOG << "[MESH] Creating SKINNED mesh - vertices: " << vertices.size() << ", indices: " << indices.size() << std::endl;
    MESH_LOG << "[MESH]   Bone weights: " << boneWeights.size() << ", Bone indices: " << boneIndices.size() << std::endl;
    _verticesCount = vertices.size();
    _indicesCount = indices.size();

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    MESH_LOG << "[MESH] Generated OpenGL buffers - VAO: " << _vao << ", VBO: " << _vbo << ", EBO: " << _ebo << std::endl;
    glBindVertexArray(_vao);

    // Upload vertices
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(float)), vertices.data(), GL_DYNAMIC_DRAW);

    // Upload indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(uint32_t)), indices.data(), GL_DYNAMIC_DRAW);

    int32_t stride = useTexture ? static_cast<int32_t>(5 * sizeof(float)) : static_cast<int32_t>(3 * sizeof(float));

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coords (location 1)
    if (useTexture) {
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // Bone weights and indices in separate VBOs
    if (!boneWeights.empty() && !boneIndices.empty()) {
        MESH_LOG << "[MESH] Setting up bone data VBOs..." << std::endl;
        uint32_t boneWeightsVBO, boneIndicesVBO;

        // Bone weights (location 2) - 4 floats per vertex
        glGenBuffers(1, &boneWeightsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, boneWeightsVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<long>(boneWeights.size() * sizeof(float)),
                     boneWeights.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);
        MESH_LOG << "[MESH] Created bone weights VBO: " << boneWeightsVBO << std::endl;

        // Bone indices (location 3) - 4 ints per vertex
        glGenBuffers(1, &boneIndicesVBO);
        glBindBuffer(GL_ARRAY_BUFFER, boneIndicesVBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<long>(boneIndices.size() * sizeof(int)),
                     boneIndices.data(), GL_STATIC_DRAW);
        glVertexAttribIPointer(3, 4, GL_INT, 4 * sizeof(int), (void *)0);
        glEnableVertexAttribArray(3);
        MESH_LOG << "[MESH] Created bone indices VBO: " << boneIndicesVBO << std::endl;
    } else {
        MESH_LOG << "[MESH] No bone data to set up (skinned mesh without bones?)" << std::endl;
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

Mesh::Mesh(
    const std::vector<float> &vertices,
    const std::vector<uint32_t> &indices,
    VertexFormat format)
{
    _verticesCount = vertices.size();
    _indicesCount = indices.size();

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(float)), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(uint32_t)), indices.data(), GL_DYNAMIC_DRAW);

    if (format == VertexFormat::PositionColor) {
        const int32_t stride = static_cast<int32_t>(6 * sizeof(float));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    } else if (format == VertexFormat::PositionTexture) {
        const int32_t stride = static_cast<int32_t>(5 * sizeof(float));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    } else {
        const int32_t stride = static_cast<int32_t>(3 * sizeof(float));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    MESH_LOG << "Mesh Destroyed " << std::endl;
    MESH_LOG << "VAO: " << _vao << " VBO: " << _vbo << std::endl;
    MESH_LOG << "\n";
#endif
#endif
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void Mesh::Bind() const noexcept
{
    glBindVertexArray(_vao);
#ifndef NDEBUG
    GLint currentVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    if (currentVAO != static_cast<GLint>(_vao)) {
        std::cerr << "[MESH] WARNING: Bind failed! Expected VAO " << _vao << " but got " << currentVAO << std::endl;
    }
#endif
}

size_t Mesh::GetVertexCount() const noexcept
{
    return _verticesCount;
}

size_t Mesh::GetIndicesCount() const noexcept
{
    return _indicesCount;
}

void Mesh::ConfigureInstanceAttributes(uint32_t instanceVBO) {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE,
                              80,
                              reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(4 + i, 1);
    }

    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(8, 1, GL_INT,
                           80,
                           reinterpret_cast<void*>(sizeof(glm::mat4)));
    glVertexAttribDivisor(8, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    _instanceAttributesConfigured = true;
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
