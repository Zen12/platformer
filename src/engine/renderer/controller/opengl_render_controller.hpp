#pragma once
#include "../render_repository.hpp"
#include <GL/glew.h>
#include <iostream>


class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};

public:

    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager)
        : _sceneManager(sceneManager) {}

    void Render(const std::shared_ptr<RenderRepository>& repository) const noexcept {
        const auto renderData = repository->GetData();

        for (const auto &[renderId, matrixVector] : renderData) {

            const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
            const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);

            if (!mat || !meshPtr) {
                continue;
            }

            mat->Bind();
            meshPtr->Bind();

            // Debug GL state
            GLint currentVAO, currentEBO;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEBO);
            static int stateDebugCounter = 0;
            if (stateDebugCounter++ == 0) {
                std::cout << "Current VAO: " << currentVAO << " (meshVAO: " << meshPtr->VAO() << ")" << std::endl;
                std::cout << "Current EBO: " << currentEBO << std::endl;
            }

            mat->SetMat4("view", renderId.CameraView);
            mat->SetMat4("projection", renderId.CameraProjection);

            // Draw each instance separately for now (will optimize later)
            for (const auto& instanceData : matrixVector) {
                mat->SetMat4("model", instanceData.ModelMatrix);

                // Set bone transforms if this is a skinned mesh
                if (instanceData.BoneTransforms.has_value()) {
                    mat->SetMat4Array("boneMatrices", instanceData.BoneTransforms.value());
                }

                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

};
