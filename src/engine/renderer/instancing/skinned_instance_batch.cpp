#include "skinned_instance_batch.hpp"

SkinnedInstanceBatch::SkinnedInstanceBatch()
    : _boneBuffer(std::make_unique<BoneMatrixBuffer>()),
      _instanceBuffer(std::make_unique<InstanceBuffer>()) {
}

void SkinnedInstanceBatch::Clear() {
    _pendingBoneData.clear();
    _pendingInstanceData.clear();
    _instanceCount = 0;
}

void SkinnedInstanceBatch::AddInstance(const glm::mat4& modelMatrix,
                                       const std::vector<glm::mat4>& boneTransforms) {
    const int32_t boneOffset = static_cast<int32_t>(_instanceCount * _maxBonesPerInstance);

    InstanceVertexData instanceData{};
    instanceData.ModelMatrix = modelMatrix;
    instanceData.BoneOffset = boneOffset;
    _pendingInstanceData.push_back(instanceData);

    const size_t bonesToCopy = std::min(boneTransforms.size(), _maxBonesPerInstance);
    for (size_t i = 0; i < bonesToCopy; ++i) {
        _pendingBoneData.push_back(boneTransforms[i]);
    }

    for (size_t i = bonesToCopy; i < _maxBonesPerInstance; ++i) {
        _pendingBoneData.push_back(glm::mat4(1.0f));
    }

    _instanceCount++;
}

void SkinnedInstanceBatch::Finalize() {
    if (_instanceCount == 0) {
        return;
    }

    _boneBuffer->Resize(_instanceCount);
    _boneBuffer->Upload(_pendingBoneData);

    _instanceBuffer->Resize(_instanceCount);
    _instanceBuffer->Upload(_pendingInstanceData);
}

void SkinnedInstanceBatch::BindForRendering(GLuint boneTextureUnit) const {
    _boneBuffer->Bind(boneTextureUnit);
}

void SkinnedInstanceBatch::SetupInstanceAttributes() const {
    if (_instanceBuffer) {
        _instanceBuffer->SetupAttributes();
    }
}
