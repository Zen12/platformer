#pragma once
#include "../render_repository.hpp"
#include "../instancing/instance_batch.hpp"
#include "../framebuffer/framebuffer.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

class SceneManager;
class Mesh;
class Shader;

class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};
    std::unordered_map<std::string, std::unique_ptr<InstanceBatch>> _batches{};

    // Skybox
    GLuint _skyboxVao{0};
    GLuint _skyboxVbo{0};
    bool _skyboxInitialized{false};

    // Post-processing
    std::unique_ptr<Framebuffer> _framebuffer{};
    GLuint _screenQuadVao{0};
    GLuint _screenQuadVbo{0};
    bool _screenQuadInitialized{false};

    static std::pair<uint16_t, uint16_t> GetViewportSize() noexcept;
    void RenderInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;
    void RenderLines(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;
    void InitSkybox();
    void RenderSkybox(const glm::mat4& view, const glm::mat4& projection, const std::string& materialGuid) noexcept;
    void InitScreenQuad();
    void RenderPostProcess() noexcept;

public:
    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager)
        : _sceneManager(sceneManager), _framebuffer(std::make_unique<Framebuffer>()) {
        const auto [width, height] = GetViewportSize();
        _framebuffer->Init(width, height);
    }

    ~OpenGLRenderController();

    void Reset() const noexcept {
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render(const std::shared_ptr<RenderRepository>& repository) noexcept;
};
