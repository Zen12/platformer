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
        std::cout << "[RENDER_CONTROLLER] Render called with " << renderData.size() << " render batches" << std::endl;

        for (const auto &[renderId, matrixVector] : renderData) {
            std::cout << "[RENDER_CONTROLLER] Processing batch - Material: " << renderId.MaterialGuid
                      << ", Mesh: " << renderId.MeshGuid << ", Instances: " << matrixVector.size() << std::endl;

            const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
            const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);

            if (!mat || !meshPtr) {
                std::cerr << "[RENDER_CONTROLLER] ERROR: Failed to get material or mesh!" << std::endl;
                std::cerr << "[RENDER_CONTROLLER]   Material valid: " << (mat ? "yes" : "no")
                          << ", Mesh valid: " << (meshPtr ? "yes" : "no") << std::endl;
                continue;
            }
            std::cout << "[RENDER_CONTROLLER] Material and mesh retrieved successfully" << std::endl;

            std::cout << "[RENDER_CONTROLLER] Binding material and mesh..." << std::endl;
            mat->Bind();
            meshPtr->Bind();

            // Debug GL state
            GLint currentVAO, currentEBO;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEBO);
            std::cout << "[RENDER_CONTROLLER] GL State - VAO: " << currentVAO
                      << " (expected: " << meshPtr->VAO() << "), EBO: " << currentEBO << std::endl;

            mat->SetMat4("view", renderId.CameraView);
            mat->SetMat4("projection", renderId.CameraProjection);

            // Draw each instance separately for now (will optimize later)
            for (size_t i = 0; i < matrixVector.size(); i++) {
                const auto& instanceData = matrixVector[i];
                mat->SetMat4("model", instanceData.ModelMatrix);

                // Set bone transforms if this is a skinned mesh
                if (instanceData.BoneTransforms.has_value()) {
                    std::cout << "[RENDER_CONTROLLER] Setting bone transforms (" << instanceData.BoneTransforms.value().size() << " bones)" << std::endl;
                    mat->SetMat4Array("boneMatrices", instanceData.BoneTransforms.value());
                }

                std::cout << "[RENDER_CONTROLLER] Drawing instance " << (i+1) << "/" << matrixVector.size()
                          << " - " << meshPtr->GetIndicesCount() << " indices" << std::endl;
                glDrawElements(GL_TRIANGLES, static_cast<int32_t>(meshPtr->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);

                // Check for GL errors
                GLenum err = glGetError();
                if (err != GL_NO_ERROR) {
                    std::cerr << "[RENDER_CONTROLLER] OpenGL ERROR after draw: " << err << std::endl;
                }
            }
        }
    }

};
