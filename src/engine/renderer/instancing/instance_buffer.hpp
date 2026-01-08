#pragma once
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <vector>

struct InstanceVertexData {
    glm::mat4 ModelMatrix;
    glm::vec4 InstanceColor;  // Per-instance color tint (RGBA)
    int32_t BoneOffset;
    int32_t Padding[3];
};

class InstanceBuffer {
private:
    GLuint _vbo = 0;
    size_t _capacity = 0;

public:
    InstanceBuffer() = default;
    ~InstanceBuffer();

    InstanceBuffer(const InstanceBuffer&) = delete;
    InstanceBuffer& operator=(const InstanceBuffer&) = delete;

    void Resize(size_t instanceCount);

    void Upload(const std::vector<InstanceVertexData>& instances);

    void SetupAttributes() const;

    [[nodiscard]] GLuint GetVBO() const noexcept { return _vbo; }
};
