#include "opengl_render_controller.hpp"
#include "../../scene/scene_manager.hpp"
#include <glm/gtc/type_ptr.hpp>

void OpenGLRenderController::Render(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto renderData = repository->GetData();

    for (const auto &[renderId, matrixVector] : renderData) {
        const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);

        if (!mat) {
            continue;
        }

        const GLenum primitiveType = static_cast<GLenum>(renderId.Primitive);

        mat->Bind();
        mat->SetMat4("view", renderId.CameraView);
        mat->SetMat4("projection", renderId.CameraProjection);

        // Draw each instance separately for now (will optimize later)
        for (size_t i = 0; i < matrixVector.size(); i++) {
            const auto& instanceData = matrixVector[i];
            mat->SetMat4("model", instanceData.ModelMatrix);

            // Set bone transforms if this is a skinned mesh
            if (instanceData.BoneTransforms.has_value()) {
                const auto& bones = instanceData.BoneTransforms.value();
                mat->SetMat4Array("boneMatrices", bones);
            }

            // Check if we have direct vertex data (for lines) or a mesh
            if (instanceData.Vertices.has_value()) {
                // Direct vertex rendering (for lines)
                const auto& vertices = instanceData.Vertices.value();
                const int vertexCount = static_cast<int>(vertices.size() / 3);

                // Set line color to red
                mat->SetVec4("lineColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

                GLuint vao, vbo;
                glGenVertexArrays(1, &vao);
                glGenBuffers(1, &vbo);

                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
                glEnableVertexAttribArray(0);

                glDrawArrays(primitiveType, 0, vertexCount);

                glDeleteVertexArrays(1, &vao);
                glDeleteBuffers(1, &vbo);
            } else {
                // Mesh rendering (traditional path)
                const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);
                if (!meshPtr) {
                    continue;
                }

                meshPtr->Bind();
                glDrawElements(primitiveType, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }
}
