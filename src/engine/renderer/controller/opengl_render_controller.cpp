#include "opengl_render_controller.hpp"
#include "../../scene/scene_manager.hpp"
#include "../material/shader.hpp"
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

    constexpr const char* POST_PROCESS_MATERIAL_GUID = "0b67e64e-d1f7-4e7b-89f8-b9fdf724fc2b";
    constexpr const char* DEPTH_MATERIAL_GUID = "04a6ec12-6609-4aad-8cc2-cd892c74b4fc";
    constexpr const char* SKINNED_DEPTH_MATERIAL_GUID = "e6ea918b-1491-4c4f-b42a-90f9c322388a";
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

    const auto mat = _sceneManager->GetMaterial(POST_PROCESS_MATERIAL_GUID);
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

void OpenGLRenderController::RenderSkybox(const glm::mat4& view, const glm::mat4& projection, const std::string& materialGuid) noexcept {
    if (materialGuid.empty()) return;

    if (!_skyboxInitialized) {
        InitSkybox();
    }

    const auto mat = _sceneManager->GetMaterial(materialGuid);
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

    const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
    if (!mat) return;

    const std::string batchKey = renderId.MaterialGuid + "_" + renderId.MeshGuid;

    if (_batches.find(batchKey) == _batches.end()) {
        _batches[batchKey] = std::make_unique<InstanceBatch>();
    }

    auto& batch = _batches[batchKey];
    batch->Clear();

    for (const auto& inst : instances) {
        if (renderId.IsSkinned && inst.BoneTransforms.has_value() && !inst.BoneTransforms.value().empty()) {
            batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value());
        } else {
            batch->AddInstance(inst.ModelMatrix);
        }
    }

    batch->Finalize();

    if (batch->GetInstanceCount() == 0) {
        return;
    }

    const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);
    if (!meshPtr) {
        return;
    }

    meshPtr->Bind();
    batch->SetupInstanceAttributes();

    mat->UseShader();
    mat->SetMat4("view", renderId.CameraView);
    mat->SetMat4("projection", renderId.CameraProjection);
    mat->ApplyRenderState();

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

    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(meshPtr->GetIndicesCount()),
        GL_UNSIGNED_INT,
        nullptr,
        static_cast<GLsizei>(batch->GetInstanceCount())
    );
}

void OpenGLRenderController::RenderLines(const RenderId& renderId, const std::vector<InstanceData>& instances) noexcept {
    const auto mat = _sceneManager->GetMaterial(renderId.MaterialGuid);
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

void OpenGLRenderController::RenderShadowPass(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto& lightData = repository->GetDirectionalLight();
    _hasDirectionalLight = lightData.HasLight;
    if (!lightData.HasLight) {
        return;
    }

    _lightView = lightData.View;
    _lightProjection = lightData.Projection;

    const auto depthMat = _sceneManager->GetMaterial(DEPTH_MATERIAL_GUID);
    const auto skinnedDepthMat = _sceneManager->GetMaterial(SKINNED_DEPTH_MATERIAL_GUID);
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

        const std::string batchKey = std::string("shadow_") + renderId.MeshGuid + (renderId.IsSkinned ? "_skinned" : "");

        if (_batches.find(batchKey) == _batches.end()) {
            _batches[batchKey] = std::make_unique<InstanceBatch>();
        }

        auto& batch = _batches[batchKey];
        batch->Clear();

        for (const auto& inst : instances) {
            if (renderId.IsSkinned && inst.BoneTransforms.has_value() && !inst.BoneTransforms.value().empty()) {
                batch->AddInstance(inst.ModelMatrix, inst.BoneTransforms.value());
            } else {
                batch->AddInstance(inst.ModelMatrix);
            }
        }

        batch->Finalize();

        if (batch->GetInstanceCount() == 0) {
            continue;
        }

        const auto meshPtr = _sceneManager->GetMesh(renderId.MeshGuid);
        if (!meshPtr) {
            continue;
        }

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

void OpenGLRenderController::Render(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto [width, height] = GetViewportSize();

    RenderShadowPass(repository);

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

    for (const auto &[renderId, instance] : renderData) {
        if (renderId.Primitive == PrimitiveType::Triangles) {
            RenderInstanced(renderId, instance);
        } else {
            RenderLines(renderId, instance);
        }
    }

    _framebuffer->Unbind();
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderPostProcess();
}
