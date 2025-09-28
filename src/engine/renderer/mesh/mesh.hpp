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
        const std::vector<uint32_t> &indices,
        const bool& useTexture);

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) noexcept = delete;
    Mesh& operator=(Mesh&&) noexcept = delete;

    void UpdateData(const std::vector<float> &vertices, const std::vector<uint32_t> &indices) noexcept;

    ~Mesh();

    void Bind() const noexcept;
    [[nodiscard]] size_t GetVertexCount() const noexcept;
    [[nodiscard]] size_t GetIndicesCount() const noexcept;

    [[nodiscard]] const std::vector<float> &GetVertices() const noexcept {
        return _vertices;
    }

    static Mesh GenerateSquare();
    static std::unique_ptr<Mesh> GenerateSpritePtr();

    static Mesh GenerateSprite();
    static Mesh GenerateUI();
};