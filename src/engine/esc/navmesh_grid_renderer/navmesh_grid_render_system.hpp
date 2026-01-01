#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include "../esc_core.hpp"
#include "../camera/camera_component.hpp"
#include "../../renderer/material/shader.hpp"
#include "../../navigation/navigation_manager.hpp"
#include "../../scene/scene.hpp"

class NavmeshGridRenderSystem final : public ISystem {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    std::weak_ptr<Scene> _scene;
    std::shared_ptr<Shader> _shader;

    GLuint _vao = 0;
    GLuint _vbo = 0;
    int _vertexCount = 0;
    bool _initialized = false;

    void InitializeGridGeometry() {
        if (const auto scene = _scene.lock()) {
            if (const auto navManager = scene->GetNavigationManager()) {
                if (const auto navmesh = navManager->GetNavmesh()) {
                    const int width = navmesh->GetWidth();
                    const int height = navmesh->GetHeight();
                    const int depth = navmesh->GetDepth();
                    const float cellSize = navmesh->GetCellSize();
                    const glm::vec3 origin = navmesh->GetOrigin();

                    std::vector<glm::vec3> vertices;
                    const float lineWidth = 0.3f; // Very thick lines for visibility

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

                    _vertexCount = static_cast<int>(vertices.size());

                    std::cout << "[NavmeshGridRenderer] Generated " << _vertexCount << " vertices for thick lines" << std::endl;

                    glGenVertexArrays(1, &_vao);
                    glGenBuffers(1, &_vbo);

                    glBindVertexArray(_vao);
                    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
                    glEnableVertexAttribArray(0);

                    glBindVertexArray(0);

                    _initialized = true;
                }
            }
        }
    }

public:
    explicit NavmeshGridRenderSystem(
        const TypeCamera &cameraView,
        const std::weak_ptr<Scene> &scene)
        : _cameraView(cameraView), _scene(scene) {

        if (const auto scenePtr = _scene.lock()) {
            _shader = scenePtr->GetShader(
                "b4b7d74c-0a2b-4d07-bba4-eb45086c7644",
                "bdf51932-9744-4c61-808b-b244713138b8"
            );

            if (!_shader) {
                std::cout << "[NavmeshGridRenderer] ERROR: Failed to load shader!" << std::endl;
            }
        }

        InitializeGridGeometry();
    }

    ~NavmeshGridRenderSystem() override {
        if (_vao != 0) {
            glDeleteVertexArrays(1, &_vao);
        }
        if (_vbo != 0) {
            glDeleteBuffers(1, &_vbo);
        }
    }

    void OnTick() override {
        if (!_initialized || !_shader) {
            return;
        }

        static bool firstRender = true;

        for (const auto &[_, camera] : _cameraView.each()) {
            if (firstRender) {
                std::cout << "[NavmeshGridRenderer] Rendering grid with " << _vertexCount << " vertices" << std::endl;
                std::cout << "[NavmeshGridRenderer] Shader program ID: " << _shader->GetShaderId() << std::endl;
                std::cout << "[NavmeshGridRenderer] VAO: " << _vao << ", VBO: " << _vbo << std::endl;
                firstRender = false;
            }

            GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
            GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE); // Disable backface culling

            _shader->Use();

            const glm::mat4 model = glm::mat4(1.0f);
            const int32_t modelLoc = _shader->GetLocation("model");
            const int32_t viewLoc = _shader->GetLocation("view");
            const int32_t projectionLoc = _shader->GetLocation("projection");
            const int32_t colorLoc = _shader->GetLocation("gridColor");

            _shader->SetMat4(modelLoc, model);
            _shader->SetMat4(viewLoc, camera.View);
            _shader->SetMat4(projectionLoc, camera.Projection);
            _shader->SetVec4(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White lines for testing

            glBindVertexArray(_vao);

            if (firstRender) {
                GLenum err = glGetError();
                if (err != GL_NO_ERROR) {
                    std::cout << "[NavmeshGridRenderer] OpenGL error before draw: " << err << std::endl;
                }
            }

            glDrawArrays(GL_TRIANGLES, 0, _vertexCount);

            if (firstRender) {
                GLenum err = glGetError();
                if (err != GL_NO_ERROR) {
                    std::cout << "[NavmeshGridRenderer] OpenGL error after draw: " << err << std::endl;
                } else {
                    std::cout << "[NavmeshGridRenderer] Draw call succeeded, no GL errors" << std::endl;
                }
            }

            glBindVertexArray(0);

            if (depthTestEnabled) {
                glEnable(GL_DEPTH_TEST);
            }
            if (cullFaceEnabled) {
                glEnable(GL_CULL_FACE);
            }
        }
    }
};
