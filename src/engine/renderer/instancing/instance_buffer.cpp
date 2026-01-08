#include "instance_buffer.hpp"
#include <glm/gtc/type_ptr.hpp>

InstanceBuffer::~InstanceBuffer() {
    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
}

void InstanceBuffer::Resize(size_t instanceCount) {
    if (instanceCount <= _capacity && _vbo != 0) {
        return;
    }

    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
    }

    _capacity = instanceCount;

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(_capacity * sizeof(InstanceVertexData)),
                 nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstanceBuffer::Upload(const std::vector<InstanceVertexData>& instances) {
    if (_vbo == 0 || instances.empty()) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(instances.size() * sizeof(InstanceVertexData)),
                    instances.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstanceBuffer::SetupAttributes() const {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Attributes 4-7: ModelMatrix (4x vec4)
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE,
                              sizeof(InstanceVertexData),
                              reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(4 + i, 1);
    }

    // Attribute 8: InstanceColor (vec4)
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE,
                          sizeof(InstanceVertexData),
                          reinterpret_cast<void*>(sizeof(glm::mat4)));
    glVertexAttribDivisor(8, 1);

    // Attribute 9: BoneOffset (int)
    glEnableVertexAttribArray(9);
    glVertexAttribIPointer(9, 1, GL_INT,
                           sizeof(InstanceVertexData),
                           reinterpret_cast<void*>(sizeof(glm::mat4) + sizeof(glm::vec4)));
    glVertexAttribDivisor(9, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
