#pragma once
#include "buffer/render_buffer.hpp"
#include "../instancing/instance_batch.hpp"
#include "../framebuffer/framebuffer.hpp"
#include "../framebuffer/depth_framebuffer.hpp"
#include "guid.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

class SceneManager;
class ResourceFactory;
class Mesh;
class Shader;

class OpenGLRenderController final {
    std::shared_ptr<SceneManager> _sceneManager{};
    std::shared_ptr<ResourceFactory> _resourceFactory{};
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

    // Shadow mapping
    std::unique_ptr<DepthFramebuffer> _shadowMap{};
    static constexpr uint16_t SHADOW_MAP_SIZE = 2048;
    bool _hasDirectionalLight{false};
    glm::mat4 _lightView{1.0f};
    glm::mat4 _lightProjection{1.0f};

    // Spot light shadow mapping
    std::array<std::unique_ptr<DepthFramebuffer>, MAX_SPOT_LIGHTS> _spotShadowMaps{};
    static constexpr uint16_t SPOT_SHADOW_MAP_SIZE = 1024;
    std::array<SpotLightData, MAX_SPOT_LIGHTS> _spotLights{};
    int _numSpotLights{0};

    // Scene depth copy (for sampling depth in transparent shaders)
    GLuint _depthCopyFbo{0};
    GLuint _depthCopyTexture{0};
    uint16_t _depthCopyWidth{0};
    uint16_t _depthCopyHeight{0};

    static std::pair<uint16_t, uint16_t> GetViewportSize() noexcept;
    void RenderInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;
    void RenderLines(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;
    void InitSkybox();
    void RenderSkybox(const glm::mat4& view, const glm::mat4& projection, const Guid& materialGuid) noexcept;
    void InitScreenQuad();
    void RenderPostProcess() noexcept;
    void RenderShadowPass(const std::shared_ptr<RenderBuffer>& repository) noexcept;
    void RenderSpotShadowPass(const std::shared_ptr<RenderBuffer>& repository) noexcept;
    void CopySceneDepth() noexcept;

public:
    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager, const std::shared_ptr<ResourceFactory> &resourceFactory)
        : _sceneManager(sceneManager), _resourceFactory(resourceFactory),
          _framebuffer(std::make_unique<Framebuffer>()),
          _shadowMap(std::make_unique<DepthFramebuffer>()) {
        const auto [width, height] = GetViewportSize();
        _framebuffer->Init(width, height);
        _shadowMap->Init(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        for (auto& sm : _spotShadowMaps) {
            sm = std::make_unique<DepthFramebuffer>();
            sm->Init(SPOT_SHADOW_MAP_SIZE, SPOT_SHADOW_MAP_SIZE);
        }
    }

    ~OpenGLRenderController();

    void Reset() const noexcept {
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render(const std::shared_ptr<RenderBuffer>& repository) noexcept;
};
