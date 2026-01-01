#include "opengl_render_controller.hpp"
#include "../../scene/scene.hpp"
#include "../../scene/scene_manager.hpp"
#include "../../navigation/navigation_manager.hpp"
#include <vector>

void OpenGLRenderController::Render(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto renderData = repository->GetData();

    glm::mat4 cameraView;
    glm::mat4 cameraProjection;
    bool hasCameraData = false;

    for (const auto &[renderId, matrixVector] : renderData) {
        const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
        const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);

        if (!mat || !meshPtr) {
            continue;
        }

        // Store camera data for navmesh grid rendering
        if (!hasCameraData) {
            cameraView = renderId.CameraView;
            cameraProjection = renderId.CameraProjection;
            hasCameraData = true;
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

    // Render navmesh grid if we have camera data
    if (hasCameraData) {
        RenderNavmeshGrid(cameraView, cameraProjection);
    }
}

void OpenGLRenderController::RenderNavmeshGrid(const glm::mat4& view, const glm::mat4& projection) {
    // Initialize grid geometry if needed
    if (!_gridInitialized) {
        InitializeNavmeshGrid();
    }

    if (!_gridInitialized || _gridVertexCount == 0) {
        return;
    }

    // Get grid shader
    const auto shader = _sceneManager->GetShader(
        "b4b7d74c-0a2b-4d07-bba4-eb45086c7644",
        "bdf51932-9744-4c61-808b-b244713138b8"
    );

    if (!shader) {
        return;
    }

    // Render grid
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    shader->Use();

    const glm::mat4 model = glm::mat4(1.0f);
    shader->SetMat4(shader->GetLocation("model"), model);
    shader->SetMat4(shader->GetLocation("view"), view);
    shader->SetMat4(shader->GetLocation("projection"), projection);
    shader->SetVec4(shader->GetLocation("gridColor"), 0.0f, 0.0f, 0.0f, 1.0f);

    glBindVertexArray(_gridVAO);
    glDrawArrays(GL_TRIANGLES, 0, _gridVertexCount);
    glBindVertexArray(0);

    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
    if (cullFaceEnabled) {
        glEnable(GL_CULL_FACE);
    }
}

void OpenGLRenderController::InitializeNavmeshGrid() {
    if (const auto scene = _sceneManager->GetScene()) {
        if (const auto navManager = scene->GetNavigationManager()) {
            if (const auto navmesh = navManager->GetNavmesh()) {
                const int width = navmesh->GetWidth();
                const int depth = navmesh->GetDepth();
                const float cellSize = navmesh->GetCellSize();
                const glm::vec3 origin = navmesh->GetOrigin();

                std::vector<glm::vec3> vertices;
                const float lineWidth = 0.3f; // Thick lines for visibility

                // Create thick lines as rectangles
                for (int x = 0; x <= width; ++x) {
                    for (int z = 0; z <= depth; ++z) {
                        const float worldX = origin.x + x * cellSize;
                        const float worldY = 0.0f; // Ground level
                        const float worldZ = origin.z + z * cellSize;

                        // Horizontal lines (along X axis)
                        if (x < width) {
                            // Create a quad for thick line
                            vertices.push_back(glm::vec3(worldX, worldY, worldZ - lineWidth));
                            vertices.push_back(glm::vec3(worldX + cellSize, worldY, worldZ - lineWidth));
                            vertices.push_back(glm::vec3(worldX + cellSize, worldY, worldZ + lineWidth));

                            vertices.push_back(glm::vec3(worldX, worldY, worldZ - lineWidth));
                            vertices.push_back(glm::vec3(worldX + cellSize, worldY, worldZ + lineWidth));
                            vertices.push_back(glm::vec3(worldX, worldY, worldZ + lineWidth));
                        }

                        // Vertical lines (along Z axis)
                        if (z < depth) {
                            // Create a quad for thick line
                            vertices.push_back(glm::vec3(worldX - lineWidth, worldY, worldZ));
                            vertices.push_back(glm::vec3(worldX + lineWidth, worldY, worldZ));
                            vertices.push_back(glm::vec3(worldX + lineWidth, worldY, worldZ + cellSize));

                            vertices.push_back(glm::vec3(worldX - lineWidth, worldY, worldZ));
                            vertices.push_back(glm::vec3(worldX + lineWidth, worldY, worldZ + cellSize));
                            vertices.push_back(glm::vec3(worldX - lineWidth, worldY, worldZ + cellSize));
                        }
                    }
                }

                _gridVertexCount = static_cast<int>(vertices.size());

                glGenVertexArrays(1, &_gridVAO);
                glGenBuffers(1, &_gridVBO);

                glBindVertexArray(_gridVAO);
                glBindBuffer(GL_ARRAY_BUFFER, _gridVBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
                glEnableVertexAttribArray(0);

                glBindVertexArray(0);

                _gridInitialized = true;
            }
        }
    }
}
