#pragma once
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <vector>

struct StaticInstanceVertexData {
    glm::mat4 ModelMatrix;
};

class StaticInstanceBatch {
private:
    GLuint _vbo = 0;
    size_t _capacity = 0;
    size_t _instanceCount = 0;
    std::vector<StaticInstanceVertexData> _pendingData;

public:
    StaticInstanceBatch() = default;
    ~StaticInstanceBatch();

    StaticInstanceBatch(const StaticInstanceBatch&) = delete;
    StaticInstanceBatch& operator=(const StaticInstanceBatch&) = delete;

    void Clear();
    void AddInstance(const glm::mat4& modelMatrix);
    void Finalize();

    [[nodiscard]] size_t GetInstanceCount() const noexcept { return _instanceCount; }
    [[nodiscard]] GLuint GetVBO() const noexcept { return _vbo; }

    void SetupInstanceAttributes() const;
};
