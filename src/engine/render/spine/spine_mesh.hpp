#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <iostream>

class SpineMesh
{
public:
private:
    uint32_t _vbo{};
    uint32_t _vao{};
    uint32_t _ebo{};
    std::vector<float> _vertices;
    std::vector<uint32_t> _indices;

public:
    SpineMesh() = delete;
    SpineMesh(
        const std::vector<float> &vertices,
        const std::vector<uint32_t> &indices,
        const bool& useTexture);

    SpineMesh(const SpineMesh&) = delete;
    SpineMesh& operator=(const SpineMesh&) = delete;

    SpineMesh(SpineMesh&&) noexcept = delete;
    SpineMesh& operator=(SpineMesh&&) noexcept = delete;

    ~SpineMesh();

    void Bind() const noexcept;
    [[nodiscard]] size_t GetVertexCount() const noexcept;
    [[nodiscard]] size_t GetIndicesCount() const noexcept;

    [[nodiscard]] static SpineMesh Create();

};