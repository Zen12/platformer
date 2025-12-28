#pragma once
#include "../render_repository.hpp"
#include <GL/glew.h>
#include <iostream>


class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};

public:

    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager)
        : _sceneManager(sceneManager) {}

    void Reset() const noexcept {
        // Always clear the framebuffer at the start of each frame
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);  // Light blue (sky blue)
        glDepthMask(GL_TRUE);  // Ensure depth writes are enabled before clearing depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear depth buffer for 3D rendering
    }

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

                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

};
