#pragma once
#include "../render_repository.hpp"
#include "../instancing/skinned_instance_batch.hpp"
#include "../instancing/static_instance_batch.hpp"
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
    std::unordered_map<std::string, std::unique_ptr<StaticInstanceBatch>> _staticBatches{};


    // Skybox
    GLuint _skyboxVao{0};
    GLuint _skyboxVbo{0};
    bool _skyboxInitialized{false};

    void RenderSkinnedInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;
    void RenderStaticInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept;
    void InitSkybox();
    void RenderSkybox(const glm::mat4& view, const glm::mat4& projection, const std::string& materialGuid) noexcept;

public:
    explicit OpenGLRenderController(const std::shared_ptr<SceneManager> &sceneManager)
        : _sceneManager(sceneManager) {}

    ~OpenGLRenderController();

    void Reset() const noexcept {
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render(const std::shared_ptr<RenderRepository>& repository) noexcept;
};
