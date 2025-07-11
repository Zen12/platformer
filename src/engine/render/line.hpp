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
    Line() = default;
    Line(const std::vector<float> &vertices);

    ~Line();

    void Bind() const noexcept;
    [[nodiscard]] size_t GetVertexCount() const noexcept;
    [[nodiscard]] size_t GetIndicesCount() const noexcept;

    void UpdateVertices(const std::vector<float> &vertices) const;

    static Line Generate();
};
