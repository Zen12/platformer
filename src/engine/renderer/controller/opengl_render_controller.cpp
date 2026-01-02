#include "opengl_render_controller.hpp"
#include "../../scene/scene_manager.hpp"
#include <glm/gtc/type_ptr.hpp>

void OpenGLRenderController::Render(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto renderData = repository->GetData();

    for (const auto &[renderId, instance] : renderData) {
        const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);

        if (!mat) {
            continue;
        }

        const auto primitiveType = renderId.Primitive;

        mat->Bind();
        mat->SetMat4("view", renderId.CameraView);
        mat->SetMat4("projection", renderId.CameraProjection);

        for (size_t i = 0; i < instance.size(); i++) {
            const auto& instanceData = instance[i];
            mat->SetMat4("model", instanceData.ModelMatrix);

            // Set bone transforms if this is a skinned mesh
            if (instanceData.BoneTransforms.has_value()) {
                const auto& bones = instanceData.BoneTransforms.value();
                mat->SetMat4Array("boneMatrices", bones);
            }

            if (primitiveType == PrimitiveType::Lines) {
                // Direct position rendering (for lines)
                const auto& positions = instanceData.Positions.value();
                const int vertexCount = static_cast<int>(positions.size());

                // Convert positions to vertex data (flat float array)
                std::vector<float> vertices;
                vertices.reserve(vertexCount * 3);
                for (const auto& pos : positions) {
                    vertices.push_back(pos.x);
                    vertices.push_back(pos.y);
                    vertices.push_back(pos.z);
                }

                // Set line color from InstanceData or use default white
                const auto lineColor = instanceData.LineColor.value_or(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                mat->SetVec4("lineColor", lineColor);

                GLuint vao, vbo;
                glGenVertexArrays(1, &vao);
                glGenBuffers(1, &vbo);

                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
                glEnableVertexAttribArray(0);

                glDrawArrays(GL_LINES, 0, vertexCount);

                glDeleteVertexArrays(1, &vao);
                glDeleteBuffers(1, &vbo);
            } else {
                const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);
                if (!meshPtr) {
                    continue;
                }

                meshPtr->Bind();
                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }
}
