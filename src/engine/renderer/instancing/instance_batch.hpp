#pragma once
#include "bone_matrix_buffer.hpp"
#include "instance_buffer.hpp"
#include <memory>
#include <vector>

class InstanceBatch {
private:
    std::unique_ptr<BoneMatrixBuffer> _boneBuffer;
    std::unique_ptr<InstanceBuffer> _instanceBuffer;

    std::vector<glm::mat4> _pendingBoneData;
    std::vector<InstanceVertexData> _pendingInstanceData;

    size_t _instanceCount = 0;
    size_t _maxBonesPerInstance = 100;
    bool _hasBones = false;

public:
    InstanceBatch();
    ~InstanceBatch() = default;

    InstanceBatch(const InstanceBatch&) = delete;
    InstanceBatch& operator=(const InstanceBatch&) = delete;

    void Clear();

    void AddInstance(const glm::mat4& modelMatrix, const glm::vec4& color = glm::vec4(1.0f));
    void AddInstance(const glm::mat4& modelMatrix, const std::vector<glm::mat4>& boneTransforms, const glm::vec4& color = glm::vec4(1.0f));

    void Finalize();

    [[nodiscard]] size_t GetInstanceCount() const noexcept { return _instanceCount; }
    [[nodiscard]] bool HasBones() const noexcept { return _hasBones; }

    void BindBoneTexture(GLuint boneTextureUnit) const;
    void SetupInstanceAttributes() const;
};
