#include "bone_matrix_buffer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

BoneMatrixBuffer::~BoneMatrixBuffer() {
    if (_textureId) {
        glDeleteTextures(1, &_textureId);
        _textureId = 0;
    }
}

void BoneMatrixBuffer::Resize(size_t instanceCount) {
    const size_t requiredCapacity = instanceCount * _maxBonesPerInstance;

    if (requiredCapacity <= _capacity && _textureId != 0) {
        return;
    }

    if (_textureId) {
        glDeleteTextures(1, &_textureId);
    }

    _capacity = requiredCapacity;

    // Use wider texture to stay within WebGL limits
    // Each bone = 4 floats x 4 columns = 16 floats = 4 RGBA pixels
    // Pack multiple bones per row: width = bonesPerRow * 4
    constexpr int MAX_TEXTURE_SIZE = 4096;
    constexpr int PIXELS_PER_BONE = 4;

    _bonesPerRow = MAX_TEXTURE_SIZE / PIXELS_PER_BONE;  // 1024 bones per row
    _textureWidth = _bonesPerRow * PIXELS_PER_BONE;     // 4096 pixels wide
    _textureHeight = static_cast<int>((_capacity + _bonesPerRow - 1) / _bonesPerRow);  // Ceil division

    if (_textureHeight > MAX_TEXTURE_SIZE) {
        _textureHeight = MAX_TEXTURE_SIZE;  // Clamp (will lose some bones)
    }

    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
                 _textureWidth, _textureHeight,
                 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void BoneMatrixBuffer::Upload(const std::vector<glm::mat4>& allBoneMatrices) {
    if (_textureId == 0 || allBoneMatrices.empty() || _bonesPerRow == 0) {
        return;
    }

    const int numBones = static_cast<int>(allBoneMatrices.size());
    const int rowsNeeded = (numBones + _bonesPerRow - 1) / _bonesPerRow;  // ceil division
    const int totalPixels = rowsNeeded * _textureWidth;

    // Prepare texture data with padding for complete rows
    // Layout: Row 0 has bones 0..1023, Row 1 has bones 1024..2047, etc.
    // Each bone = 4 RGBA pixels (16 floats for mat4)
    std::vector<float> textureData(totalPixels * 4, 0.0f);

    for (size_t i = 0; i < allBoneMatrices.size(); ++i) {
        const float* ptr = glm::value_ptr(allBoneMatrices[i]);
        size_t floatOffset = i * 16;  // 16 floats per bone
        for (int j = 0; j < 16; ++j) {
            textureData[floatOffset + j] = ptr[j];
        }
    }

    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,
                    _textureWidth, rowsNeeded,
                    GL_RGBA, GL_FLOAT, textureData.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BoneMatrixBuffer::Bind(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, _textureId);
}
