#pragma once
#include "../render_repository.hpp"


class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};

public:

    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager)
        : _sceneManager(sceneManager) {}

    void Render(const std::shared_ptr<RenderRepository>& repository) const {
        const auto renderData = repository->GetData();

        for (const auto &[renderId, matrixVector] : renderData) {

            const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
            const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);

            mat->Bind();

            mat->SetMat4("view", renderId.CameraProjection);
            mat->SetMat4("projection", renderId.CameraView);

            // can be instance rendering
            for (const auto &matrix : matrixVector) {

                mat->SetMat4("model", matrix);
                meshPtr->Bind();
                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

};
