#pragma once
#include <vector>

#include "glm/fwd.hpp"
#include "GL/glew.h"


class Line {
private:
    uint32_t _vbo{};
    uint32_t _vao{};
    std::vector<float> _vertices;
    std::vector<uint32_t> _indices;

public:
    Line() = delete;

    explicit Line(const std::vector<float> &vertices);

    Line(const Line&) = delete;
    Line& operator=(const Line&) = delete;

    Line(Line&&) = delete;
    Line& operator=(Line&&) = delete;

    ~Line();

    void Bind() const noexcept;
    [[nodiscard]] size_t GetVertexCount() const noexcept;
    [[nodiscard]] size_t GetIndicesCount() const noexcept;

    void UpdateVertices(const std::vector<float> &vertices);

    static Line Generate();
};
