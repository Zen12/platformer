#pragma once
#include "../render_repository.hpp"
#include <GL/glew.h>
#include <iostream>
#include <glm/glm.hpp>

class SceneManager;

class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};

    // Navmesh grid rendering
    GLuint _gridVAO = 0;
    GLuint _gridVBO = 0;
    int _gridVertexCount = 0;
    bool _gridInitialized = false;

public:

    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager)
        : _sceneManager(sceneManager) {}

    void Reset() const noexcept {
        // Always clear the framebuffer at the start of each frame
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);  // Light blue (sky blue)
        glDepthMask(GL_TRUE);  // Ensure depth writes are enabled before clearing depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear depth buffer for 3D rendering
    }

    void Render(const std::shared_ptr<RenderRepository>& repository) noexcept;
    void RenderNavmeshGrid(const glm::mat4& view, const glm::mat4& projection);

    ~OpenGLRenderController() {
        if (_gridVAO != 0) {
            glDeleteVertexArrays(1, &_gridVAO);
        }
        if (_gridVBO != 0) {
            glDeleteBuffers(1, &_gridVBO);
        }
    }

private:
    void InitializeNavmeshGrid();
};
