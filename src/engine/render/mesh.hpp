#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <iostream>

class Mesh
{
public:
private:
    uint32_t _vbo{};
    uint32_t _vao{};
    uint32_t _ebo{};
    std::vector<float> _vertices;
    std::vector<uint32_t> _indices;

public:
    Mesh() = delete;
    Mesh(
        const std::vector<float> &vertices,
        const std::vector<uint32_t> &indices);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = delete;
    Mesh& operator=(Mesh&&) noexcept = delete;

    ~Mesh();

    void Bind() const noexcept;
    [[nodiscard]] size_t GetVertexCount() const noexcept;
    [[nodiscard]] size_t GetIndicesCount() const noexcept;

    static Mesh GenerateSprite();
    static Mesh GenerateUI();
};