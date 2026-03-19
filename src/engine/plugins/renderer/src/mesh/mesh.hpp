#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <iostream>

#include "RmlUi/Core/Debug.h"
#include "RmlUi/Core/Span.h"
#include "RmlUi/Core/Vertex.h"

enum class VertexFormat {
    PositionOnly,
    PositionTexture,
    PositionColor
};

class Mesh
{
public:
private:
    uint32_t _vbo{};
    uint32_t _vao{};
    uint32_t _ebo{};
    size_t _verticesCount;
    size_t _indicesCount;
    bool _instanceAttributesConfigured = false;

public:
    Mesh() = delete;
    Mesh(
        const std::vector<float> &vertices,
        const std::vector<uint32_t> &indices,
        const bool& useTexture);

    Mesh(
        const std::vector<float> &vertices,
        const std::vector<uint32_t> &indices,
        const bool& useTexture,
        const std::vector<float> &boneWeights,
        const std::vector<int> &boneIndices);

    Mesh(
    const Rml::Span<const Rml::Vertex> &vertices,
    const Rml::Span<const int> &indices);

    Mesh(
        const std::vector<float> &vertices,
        const std::vector<uint32_t> &indices,
        VertexFormat format);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = delete;
    Mesh& operator=(Mesh&&) noexcept = delete;

    void UpdateData(const std::vector<float> &vertices, const std::vector<uint32_t> &indices) noexcept;

    ~Mesh();

    void Bind() const noexcept;

    uint32_t VAO() {
        return _vao;
    }

    [[nodiscard]] size_t GetVertexCount() const noexcept;
    [[nodiscard]] size_t GetIndicesCount() const noexcept;

    void ConfigureInstanceAttributes(uint32_t instanceVBO);
    [[nodiscard]] bool HasInstanceAttributes() const noexcept { return _instanceAttributesConfigured; }

    static Mesh GenerateSquare();
    static std::unique_ptr<Mesh> GenerateSpritePtr();

    static Mesh GenerateSprite();
    static Mesh GenerateUI();
};
