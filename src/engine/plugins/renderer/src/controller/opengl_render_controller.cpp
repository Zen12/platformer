#include "opengl_render_controller.hpp"
#include "scene_manager.hpp"
#include "resource_factory.hpp"
#include "guid.hpp"
#include "profiler.hpp"
#include "../material/shader.hpp"
#include "../material/material.hpp"
#include "../mesh/mesh.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace {
    constexpr float SkyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    constexpr float ScreenQuadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // 0b67e64e-d1f7-4e7b-89f8-b9fdf724fc2b
    constexpr Guid POST_PROCESS_MATERIAL_GUID{0x0b67e64ed1f74e7b, 0x89f8b9fdf724fc2b};
    // 04a6ec12-6609-4aad-8cc2-cd892c74b4fc
    constexpr Guid DEPTH_MATERIAL_GUID{0x04a6ec1266094aad, 0x8cc2cd892c74b4fc};
    // e6ea918b-1491-4c4f-b42a-90f9c322388a
    constexpr Guid SKINNED_DEPTH_MATERIAL_GUID{0xe6ea918b14914c4f, 0xb42a90f9c322388a};
}

OpenGLRenderController::~OpenGLRenderController() {
    if (_skyboxVao != 0) {
        glDeleteVertexArrays(1, &_skyboxVao);
        glDeleteBuffers(1, &_skyboxVbo);
    }
    if (_screenQuadVao != 0) {
        glDeleteVertexArrays(1, &_screenQuadVao);
        glDeleteBuffers(1, &_screenQuadVbo);
    }
    if (_depthCopyFbo != 0) {
        glDeleteFramebuffers(1, &_depthCopyFbo);
    }
    if (_depthCopyTexture != 0) {
        glDeleteTextures(1, &_depthCopyTexture);
    }
}

std::pair<uint16_t, uint16_t> OpenGLRenderController::GetViewportSize() noexcept {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    return {static_cast<uint16_t>(viewport[2]), static_cast<uint16_t>(viewport[3])};
}

void OpenGLRenderController::InitSkybox() {
    if (_skyboxInitialized) return;

    glGenVertexArrays(1, &_skyboxVao);
    glGenBuffers(1, &_skyboxVbo);

    glBindVertexArray(_skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, _skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), SkyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _skyboxInitialized = true;
}

void OpenGLRenderController::InitScreenQuad() {
    if (_screenQuadInitialized) return;

    glGenVertexArrays(1, &_screenQuadVao);
    glGenBuffers(1, &_screenQuadVbo);

    glBindVertexArray(_screenQuadVao);
    glBindBuffer(GL_ARRAY_BUFFER, _screenQuadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadVertices), ScreenQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _screenQuadInitialized = true;
}

void OpenGLRenderController::RenderPostProcess() noexcept {
    if (!_screenQuadInitialized) {
        InitScreenQuad();
    }

    const auto mat = _resourceFactory->Get<Material>(POST_PROCESS_MATERIAL_GUID);
    if (!mat) {
        return;
    }

    mat->UseShader();
    mat->ApplyRenderState();

    _framebuffer->BindColorTexture(0);
    mat->SetInt("screenTexture", 0);

    _shadowMap->BindDepthTexture(1);
    mat->SetInt("depthTexture", 1);

    mat->SetInt("showDepth", 0);

    glBindVertexArray(_screenQuadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void OpenGLRenderController::RenderSkybox(const glm::mat4& view, const glm::mat4& projection, const Guid& materialGuid) noexcept {
    if (materialGuid.IsEmpty()) return;

    if (!_skyboxInitialized) {
        InitSkybox();
    }

    const auto mat = _resourceFactory->Get<Material>(materialGuid);
    if (!mat) {
        return;
    }

    mat->UseShader();
    mat->ApplyRenderState();
    mat->BindTextures();
    mat->SetMat4("view", view);
    mat->SetMat4("projection", projection);
    mat->SetInt("skyboxTexture", 0);

    glBindVertexArray(_skyboxVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void OpenGLRenderController::RenderInstanced(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept {
    if (instances.empty()) return;

    const auto mat = _resourceFactory->Get<Material>(renderId.MaterialGuid);
    if (!mat) {
        return;
    }

    const std::string batchKey = renderId.MaterialGuid.ToString() + "_" + renderId.MeshGuid.ToString();

    if (_batches.find(batchKey) == _batches.end()) {
        _batches[batchKey] = std::make_unique<InstanceBatch>();
    }

    auto& batch = _batches[batchKey];
    batch->Clear();

    for (const auto& inst : instances) {
        const glm::vec4 color = inst.InstanceColor.value_or(glm::vec4(1.0f));
        if (renderId.IsSkinned && inst.BoneTransforms.has_value() && !inst.BoneTransforms.value().empty()) {
            batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value(), color);
        } else {
            batch->AddInstance(inst.ModelMatrix, color);
        }
    }

    batch->Finalize();

    if (batch->GetInstanceCount() == 0) {
        return;
    }

    const auto meshPtr = _resourceFactory->Get<Mesh>(renderId.MeshGuid);

    meshPtr->Bind();
    batch->SetupInstanceAttributes();

    mat->UseShader();
    mat->SetMat4("view", renderId.CameraView);
    mat->SetMat4("projection", renderId.CameraProjection);
    mat->SetFloat("yDepthFactor", renderId.YDepthFactor);
    mat->ApplyRenderState();

    if (!instances.empty() && instances[0].LineColor.has_value()) {
        mat->SetVec4("lineColor", instances[0].LineColor.value());
    }

    if (mat->HasTextures()) {
        mat->BindTextures();
        mat->SetInt("texture1", 0);
    }

    if (batch->HasBones()) {
        constexpr GLuint BONE_TEXTURE_UNIT = 1;
        batch->BindBoneTexture(BONE_TEXTURE_UNIT);
        mat->SetInt("boneMatrices", BONE_TEXTURE_UNIT);
    }

    if (_hasDirectionalLight) {
        constexpr GLuint SHADOW_TEXTURE_UNIT = 2;
        _shadowMap->BindDepthTexture(SHADOW_TEXTURE_UNIT);
        mat->SetInt("shadowMap", SHADOW_TEXTURE_UNIT);
        mat->SetMat4("lightView", _lightView);
        mat->SetMat4("lightProjection", _lightProjection);
    }

    mat->SetInt("numSpotLights", _numSpotLights);
    for (int i = 0; i < _numSpotLights; i++) {
        const std::string idx = "[" + std::to_string(i) + "]";

        constexpr GLuint SPOT_SHADOW_BASE_UNIT = 4;
        _spotShadowMaps[i]->BindDepthTexture(SPOT_SHADOW_BASE_UNIT + i);
        mat->SetInt("spotShadowMaps" + idx, static_cast<int>(SPOT_SHADOW_BASE_UNIT + i));

        const glm::mat4 spotVP = _spotLights[i].Projection * _spotLights[i].View;
        mat->SetMat4("spotLightVP" + idx, spotVP);

        mat->SetVec3("spotLightPos" + idx, _spotLights[i].Position.x, _spotLights[i].Position.y, _spotLights[i].Position.z);
        mat->SetVec3("spotLightDir" + idx, _spotLights[i].Direction.x, _spotLights[i].Direction.y, _spotLights[i].Direction.z);
        mat->SetVec3("spotLightColor" + idx, _spotLights[i].Color.x, _spotLights[i].Color.y, _spotLights[i].Color.z);
        mat->SetFloat("spotInnerCutoff" + idx, _spotLights[i].InnerCutoff);
        mat->SetFloat("spotOuterCutoff" + idx, _spotLights[i].OuterCutoff);
        mat->SetFloat("spotLightRange" + idx, _spotLights[i].Range);
        mat->SetFloat("spotLightIntensity" + idx, _spotLights[i].Intensity);
    }

    constexpr GLuint SCENE_DEPTH_TEXTURE_UNIT = 3;
    glActiveTexture(GL_TEXTURE0 + SCENE_DEPTH_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, _depthCopyTexture);
    mat->SetInt("sceneDepth", SCENE_DEPTH_TEXTURE_UNIT);

    const glm::mat4 vp = renderId.CameraProjection * renderId.CameraView;
    const glm::mat4 invVP = glm::inverse(vp);
    mat->SetMat4("inverseVP", invVP);
    mat->SetVec2("screenSize", static_cast<float>(_framebuffer->GetWidth()), static_cast<float>(_framebuffer->GetHeight()));

    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(meshPtr->GetIndicesCount()),
        GL_UNSIGNED_INT,
        nullptr,
        static_cast<GLsizei>(batch->GetInstanceCount())
    );
}

void OpenGLRenderController::RenderLines(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept {
    const auto mat = _resourceFactory->Get<Material>(renderId.MaterialGuid);
    if (!mat) {
        return;
    }

    mat->Bind();
    mat->SetMat4("view", renderId.CameraView);
    mat->SetMat4("projection", renderId.CameraProjection);

    for (const auto& instanceData : instances) {
        mat->SetMat4("model", instanceData.ModelMatrix);

        const auto& positions = instanceData.Positions.value();
        const int vertexCount = static_cast<int>(positions.size());

        std::vector<float> vertices;
        vertices.reserve(vertexCount * 3);
        for (const auto& pos : positions) {
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
        }

        const auto lineColor = instanceData.LineColor.value_or(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mat->SetVec4("lineColor", lineColor);

        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, vertexCount);

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
}

void OpenGLRenderController::RenderShadowPass(const std::shared_ptr<RenderBuffer>& repository) noexcept {
    PROFILE_SCOPE("RenderShadowPass");
    const auto& lightData = repository->GetDirectionalLight();
    _hasDirectionalLight = lightData.HasLight;
    if (!lightData.HasLight) {
        return;
    }

    _lightView = lightData.View;
    _lightProjection = lightData.Projection;

    const auto depthMat = _resourceFactory->Get<Material>(DEPTH_MATERIAL_GUID);
    const auto skinnedDepthMat = _resourceFactory->Get<Material>(SKINNED_DEPTH_MATERIAL_GUID);
    if (!depthMat) {
        return;
    }

    _shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    const auto renderData = repository->GetData();

    for (const auto &[renderId, instances] : renderData) {
        if (renderId.Primitive != PrimitiveType::Triangles) {
            continue;
        }

        // Skip shadow casting for transparent/additive materials (particles, etc.)
        if (const auto mat = _resourceFactory->Get<Material>(renderId.MaterialGuid)) {
            const auto blendMode = mat->GetBlendMode();
            if (blendMode == BlendMode::ColorAdditive || blendMode == BlendMode::AlphaAdditive || blendMode == BlendMode::StandardAlpha) {
                continue;
            }
        }

        const std::string batchKey = std::string("shadow_") + renderId.MeshGuid.ToString() + (renderId.IsSkinned ? "_skinned" : "");

        if (_batches.find(batchKey) == _batches.end()) {
            _batches[batchKey] = std::make_unique<InstanceBatch>();
        }

        auto& batch = _batches[batchKey];
        batch->Clear();

        for (const auto& inst : instances) {
            if (renderId.IsSkinned && inst.BoneTransforms.has_value() && !inst.BoneTransforms.value().empty()) {
                batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value(), glm::vec4(1.0f));
            } else {
                batch->AddInstance(inst.ModelMatrix, glm::vec4(1.0f));
            }
        }

        batch->Finalize();

        if (batch->GetInstanceCount() == 0) {
            continue;
        }

        const auto meshPtr = _resourceFactory->Get<Mesh>(renderId.MeshGuid);

        meshPtr->Bind();
        batch->SetupInstanceAttributes();

        if (renderId.IsSkinned && skinnedDepthMat && batch->HasBones()) {
            skinnedDepthMat->UseShader();
            skinnedDepthMat->ApplyRenderState();
            skinnedDepthMat->SetMat4("lightView", lightData.View);
            skinnedDepthMat->SetMat4("lightProjection", lightData.Projection);

            constexpr GLuint BONE_TEXTURE_UNIT = 0;
            batch->BindBoneTexture(BONE_TEXTURE_UNIT);
            skinnedDepthMat->SetInt("boneMatrices", BONE_TEXTURE_UNIT);
        } else {
            depthMat->UseShader();
            depthMat->ApplyRenderState();
            depthMat->SetMat4("lightView", lightData.View);
            depthMat->SetMat4("lightProjection", lightData.Projection);
        }

        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<GLsizei>(meshPtr->GetIndicesCount()),
            GL_UNSIGNED_INT,
            nullptr,
            static_cast<GLsizei>(batch->GetInstanceCount())
        );
    }

    _shadowMap->Unbind();
}

void OpenGLRenderController::RenderSpotShadowPass(const std::shared_ptr<RenderBuffer>& repository) noexcept {
    PROFILE_SCOPE("RenderSpotShadowPass");
    _numSpotLights = repository->GetNumSpotLights();
    const auto& spotLights = repository->GetSpotLights();

    for (int li = 0; li < _numSpotLights; li++) {
        _spotLights[li] = spotLights[li];
    }

    if (_numSpotLights == 0) {
        return;
    }

    const auto depthMat = _resourceFactory->Get<Material>(DEPTH_MATERIAL_GUID);
    const auto skinnedDepthMat = _resourceFactory->Get<Material>(SKINNED_DEPTH_MATERIAL_GUID);
    if (!depthMat) {
        return;
    }

    const auto renderData = repository->GetData();

    for (int li = 0; li < _numSpotLights; li++) {
        _spotShadowMaps[li]->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        for (const auto &[renderId, instances] : renderData) {
            if (renderId.Primitive != PrimitiveType::Triangles) {
                continue;
            }

            if (const auto mat = _resourceFactory->Get<Material>(renderId.MaterialGuid)) {
                const auto blendMode = mat->GetBlendMode();
                if (blendMode == BlendMode::ColorAdditive || blendMode == BlendMode::AlphaAdditive || blendMode == BlendMode::StandardAlpha) {
                    continue;
                }
            }

            const std::string batchKey = std::string("spot_shadow_") + std::to_string(li) + "_" + renderId.MeshGuid.ToString() + (renderId.IsSkinned ? "_skinned" : "");

            if (_batches.find(batchKey) == _batches.end()) {
                _batches[batchKey] = std::make_unique<InstanceBatch>();
            }

            auto& batch = _batches[batchKey];
            batch->Clear();

            for (const auto& inst : instances) {
                if (renderId.IsSkinned && inst.BoneTransforms.has_value() && !inst.BoneTransforms.value().empty()) {
                    batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value(), glm::vec4(1.0f));
                } else {
                    batch->AddInstance(inst.ModelMatrix, glm::vec4(1.0f));
                }
            }

            batch->Finalize();

            if (batch->GetInstanceCount() == 0) {
                continue;
            }

            const auto meshPtr = _resourceFactory->Get<Mesh>(renderId.MeshGuid);

            meshPtr->Bind();
            batch->SetupInstanceAttributes();

            if (renderId.IsSkinned && skinnedDepthMat && batch->HasBones()) {
                skinnedDepthMat->UseShader();
                skinnedDepthMat->ApplyRenderState();
                skinnedDepthMat->SetMat4("lightView", _spotLights[li].View);
                skinnedDepthMat->SetMat4("lightProjection", _spotLights[li].Projection);

                constexpr GLuint BONE_TEXTURE_UNIT = 0;
                batch->BindBoneTexture(BONE_TEXTURE_UNIT);
                skinnedDepthMat->SetInt("boneMatrices", BONE_TEXTURE_UNIT);
            } else {
                depthMat->UseShader();
                depthMat->ApplyRenderState();
                depthMat->SetMat4("lightView", _spotLights[li].View);
                depthMat->SetMat4("lightProjection", _spotLights[li].Projection);
            }

            glDrawElementsInstanced(
                GL_TRIANGLES,
                static_cast<GLsizei>(meshPtr->GetIndicesCount()),
                GL_UNSIGNED_INT,
                nullptr,
                static_cast<GLsizei>(batch->GetInstanceCount())
            );
        }

        _spotShadowMaps[li]->Unbind();
    }
}

void OpenGLRenderController::CopySceneDepth() noexcept {
    const auto w = _framebuffer->GetWidth();
    const auto h = _framebuffer->GetHeight();

    if (_depthCopyFbo == 0) {
        glGenFramebuffers(1, &_depthCopyFbo);
        glGenTextures(1, &_depthCopyTexture);

        glBindTexture(GL_TEXTURE_2D, _depthCopyTexture);
#ifdef __EMSCRIPTEN__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
#endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, _depthCopyFbo);
#ifdef __EMSCRIPTEN__
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthCopyTexture, 0);
#else
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthCopyTexture, 0);
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        _depthCopyWidth = w;
        _depthCopyHeight = h;
    }

    if (_depthCopyWidth != w || _depthCopyHeight != h) {
        glBindTexture(GL_TEXTURE_2D, _depthCopyTexture);
#ifdef __EMSCRIPTEN__
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, nullptr);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
#endif
        _depthCopyWidth = w;
        _depthCopyHeight = h;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebuffer->GetFbo());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _depthCopyFbo);
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    _framebuffer->Bind();
}

void OpenGLRenderController::Render(const std::shared_ptr<RenderBuffer>& repository) noexcept {
    PROFILE_SCOPE("RenderController::Render");
    const auto [width, height] = GetViewportSize();

    RenderShadowPass(repository);
    RenderSpotShadowPass(repository);

    _framebuffer->Resize(width, height);
    _framebuffer->Bind();
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto renderData = repository->GetData();

    if (const auto scene = _sceneManager->GetScene()) {
        if (const auto skybox = scene->GetSkyboxRenderer()) {
            if (skybox->IsEnabled() && !renderData.empty()) {
                const auto& firstRenderId = renderData.begin()->first;
                RenderSkybox(firstRenderId.CameraView, firstRenderId.CameraProjection, skybox->GetMaterialGuid());
            }
        }
    }

    // Pass 1: Opaque objects
    for (const auto &[renderId, instance] : renderData) {
        if (renderId.Primitive == PrimitiveType::Triangles) {
            const auto mat = _resourceFactory->Get<Material>(renderId.MaterialGuid);
            if (mat && mat->GetBlendMode() == BlendMode::None) {
                RenderInstanced(renderId, instance);
            }
        } else {
            RenderLines(renderId, instance);
        }
    }

    // Copy depth buffer so transparent shaders can sample it
    CopySceneDepth();

    // Pass 2: Transparent objects (depth buffer is now fully populated)
    for (const auto &[renderId, instance] : renderData) {
        if (renderId.Primitive == PrimitiveType::Triangles) {
            const auto mat = _resourceFactory->Get<Material>(renderId.MaterialGuid);
            if (mat && mat->GetBlendMode() != BlendMode::None) {
                RenderInstanced(renderId, instance);
            }
        }
    }

    _framebuffer->Unbind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderPostProcess();
}
