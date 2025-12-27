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
                    const auto& bones = instanceData.BoneTransforms.value();
                    static int logCount = 0;
                    static bool loggedOnce = false;

                    if (logCount++ % 60 == 0) {
                        std::cout << "[RENDER_CONTROLLER] Setting bone transforms (" << bones.size() << " bones)" << std::endl;

                        // Log left vs right arm bones to verify they're different
                        if (!loggedOnce && bones.size() >= 12) {
                            std::cout << "[GPU UPLOAD] Left arm bones:" << std::endl;
                            std::cout << "  [6] UpperArm.L: pos(" << bones[6][3][0] << ", " << bones[6][3][1] << ", " << bones[6][3][2] << ")" << std::endl;
                            std::cout << "  [7] Forearm.L: pos(" << bones[7][3][0] << ", " << bones[7][3][1] << ", " << bones[7][3][2] << ")" << std::endl;
                            std::cout << "[GPU UPLOAD] Right arm bones:" << std::endl;
                            std::cout << "  [9] UpperArm.R: pos(" << bones[9][3][0] << ", " << bones[9][3][1] << ", " << bones[9][3][2] << ")" << std::endl;
                            std::cout << "  [10] Forearm.R: pos(" << bones[10][3][0] << ", " << bones[10][3][1] << ", " << bones[10][3][2] << ")" << std::endl;
                            loggedOnce = true;
                        }
                    }

                    mat->SetMat4Array("boneMatrices", bones);
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
