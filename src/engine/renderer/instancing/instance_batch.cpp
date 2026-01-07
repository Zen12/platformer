#include "instance_batch.hpp"

InstanceBatch::InstanceBatch()
    : _instanceBuffer(std::make_unique<InstanceBuffer>()) {
}

void InstanceBatch::Clear() {
    _pendingBoneData.clear();
    _pendingInstanceData.clear();
    _instanceCount = 0;
    _hasBones = false;
}

void InstanceBatch::AddInstance(const glm::mat4& modelMatrix) {
    InstanceVertexData instanceData{};
    instanceData.ModelMatrix = modelMatrix;
    instanceData.BoneOffset = 0;
    _pendingInstanceData.push_back(instanceData);
    _instanceCount++;
}

void InstanceBatch::AddInstance(const glm::mat4& modelMatrix, const std::vector<glm::mat4>& boneTransforms) {
    _hasBones = true;

    if (!_boneBuffer) {
        _boneBuffer = std::make_unique<BoneMatrixBuffer>();
    }

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

void InstanceBatch::Finalize() {
    if (_instanceCount == 0) {
        return;
    }

    if (_hasBones && _boneBuffer) {
        _boneBuffer->Resize(_instanceCount);
        _boneBuffer->Upload(_pendingBoneData);
    }

    _instanceBuffer->Resize(_instanceCount);
    _instanceBuffer->Upload(_pendingInstanceData);
}

void InstanceBatch::BindBoneTexture(GLuint boneTextureUnit) const {
    if (_hasBones && _boneBuffer) {
        _boneBuffer->Bind(boneTextureUnit);
    }
}

void InstanceBatch::SetupInstanceAttributes() const {
    if (_instanceBuffer) {
        _instanceBuffer->SetupAttributes();
    }
}
