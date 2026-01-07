#include "static_instance_batch.hpp"

StaticInstanceBatch::~StaticInstanceBatch() {
    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
}

void StaticInstanceBatch::Clear() {
    _pendingData.clear();
    _instanceCount = 0;
}

void StaticInstanceBatch::AddInstance(const glm::mat4& modelMatrix) {
    _pendingData.push_back({modelMatrix});
}

void StaticInstanceBatch::Finalize() {
    _instanceCount = _pendingData.size();

    if (_instanceCount == 0) {
        return;
    }

    if (_instanceCount > _capacity || _vbo == 0) {
        if (_vbo) {
            glDeleteBuffers(1, &_vbo);
        }

        _capacity = _instanceCount;
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(_capacity * sizeof(StaticInstanceVertexData)),
                     nullptr,
                     GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(_pendingData.size() * sizeof(StaticInstanceVertexData)),
                    _pendingData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void StaticInstanceBatch::SetupInstanceAttributes() const {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Model matrix uses locations 3-6 (4 vec4s for mat4)
    // Location 0 = aPos, Location 1 = aTexCoord, Location 2 = aNormal (if present)
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                              sizeof(StaticInstanceVertexData),
                              reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
