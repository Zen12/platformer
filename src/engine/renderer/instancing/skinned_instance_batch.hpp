#pragma once
#include "bone_matrix_buffer.hpp"
#include "instance_buffer.hpp"
#include <memory>
#include <vector>

class SkinnedInstanceBatch {
private:
    std::unique_ptr<BoneMatrixBuffer> _boneBuffer;
    std::unique_ptr<InstanceBuffer> _instanceBuffer;

    std::vector<glm::mat4> _pendingBoneData;
    std::vector<InstanceVertexData> _pendingInstanceData;

    size_t _instanceCount = 0;
    size_t _maxBonesPerInstance = 100;

public:
    SkinnedInstanceBatch();
    ~SkinnedInstanceBatch() = default;

    SkinnedInstanceBatch(const SkinnedInstanceBatch&) = delete;
    SkinnedInstanceBatch& operator=(const SkinnedInstanceBatch&) = delete;

    void Clear();

    void AddInstance(const glm::mat4& modelMatrix,
                     const std::vector<glm::mat4>& boneTransforms);

    void Finalize();

    [[nodiscard]] size_t GetInstanceCount() const noexcept { return _instanceCount; }

    void BindForRendering(GLuint boneTextureUnit) const;

    [[nodiscard]] GLuint GetInstanceVBO() const noexcept {
        return _instanceBuffer ? _instanceBuffer->GetVBO() : 0;
    }

    void SetupInstanceAttributes() const;
};
