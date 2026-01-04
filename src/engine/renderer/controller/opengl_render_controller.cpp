#include "opengl_render_controller.hpp"
#include "../../scene/scene_manager.hpp"
#include "../material/shader.hpp"
#include <glm/gtc/type_ptr.hpp>

void OpenGLRenderController::RenderSkinnedInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept {
    if (instances.empty()) return;

    const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
    if (!mat) return;

    const auto shader = mat->GetShader();
    if (!shader) return;

    const std::string batchKey = renderId.MaterialGuid + "_" + renderId.MeshGuid;

    if (_skinnedBatches.find(batchKey) == _skinnedBatches.end()) {
        _skinnedBatches[batchKey] = std::make_unique<SkinnedInstanceBatch>();
    }

    auto& batch = _skinnedBatches[batchKey];
    batch->Clear();

    for (const auto& inst : instances) {
        if (inst.BoneTransforms.has_value()) {
            batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value());
        }
    }

    batch->Finalize();

    if (batch->GetInstanceCount() == 0) return;

    const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);
    if (!meshPtr) return;

    meshPtr->Bind();

    if (!meshPtr->HasInstanceAttributes()) {
        meshPtr->ConfigureInstanceAttributes(batch->GetInstanceVBO());
    }

    batch->SetupInstanceAttributes();

    shader->Use();
    shader->SetMat4(shader->GetLocation("view"), renderId.CameraView);
    shader->SetMat4(shader->GetLocation("projection"), renderId.CameraProjection);

    constexpr GLuint BONE_TEXTURE_UNIT = 1;
    batch->BindForRendering(BONE_TEXTURE_UNIT);
    shader->SetInt(shader->GetLocation("boneMatrices"), BONE_TEXTURE_UNIT);

    mat->ApplyRenderState();
    mat->BindTextures();
    shader->SetInt(shader->GetLocation("texture1"), 0);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(meshPtr->GetIndicesCount()),
        GL_UNSIGNED_INT,
        nullptr,
        static_cast<GLsizei>(batch->GetInstanceCount())
    );
}

void OpenGLRenderController::Render(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto renderData = repository->GetData();

    for (const auto &[renderId, instance] : renderData) {
        if (renderId.IsSkinned && instance.size() > 1) {
            RenderSkinnedInstanced(renderId, instance);
            continue;
        }

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

            if (instanceData.BoneTransforms.has_value()) {
                const auto& bones = instanceData.BoneTransforms.value();
                mat->SetMat4Array("boneMatrices", bones);
            }

            if (primitiveType == PrimitiveType::Lines) {
                const auto& positions = instanceData.Positions.value();
                const int vertexCount = static_cast<int>(positions.size());

                std::vector<float> vertices;
                vertices.reserve(vertexCount * 3);
                for (const auto& pos : positions) {
                    vertices.push_back(pos.x);
                    vertices.push_back(pos.y);
                    vertices.push_back(pos.z);
                }

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
