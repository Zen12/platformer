#pragma once
#include "../render_repository.hpp"
#include "../instancing/skinned_instance_batch.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

class SceneManager;
class Mesh;
class Shader;

class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};
    std::unordered_map<std::string, std::unique_ptr<SkinnedInstanceBatch>> _skinnedBatches{};
    std::shared_ptr<Shader> _instancedSkinnedShader{};
    bool _instancedShaderLoaded = false;

    void LoadInstancedShader();
    void RenderSkinnedInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;

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
};
