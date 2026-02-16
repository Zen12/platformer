#pragma once
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <vector>

class BoneMatrixBuffer {
private:
    GLuint _textureId = 0;
    size_t _capacity = 0;
    size_t _maxBonesPerInstance = 100;
    int _textureWidth = 0;
    int _textureHeight = 0;
    int _bonesPerRow = 0;

public:
    BoneMatrixBuffer() = default;
    ~BoneMatrixBuffer();

    BoneMatrixBuffer(const BoneMatrixBuffer&) = delete;
    BoneMatrixBuffer& operator=(const BoneMatrixBuffer&) = delete;

    void Resize(size_t instanceCount);

    void Upload(const std::vector<glm::mat4>& allBoneMatrices);

    void Bind(GLuint textureUnit) const;

    [[nodiscard]] GLuint GetTextureId() const noexcept { return _textureId; }
    [[nodiscard]] size_t GetMaxBonesPerInstance() const noexcept { return _maxBonesPerInstance; }
};
