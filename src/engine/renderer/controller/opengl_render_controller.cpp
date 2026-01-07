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
}

OpenGLRenderController::~OpenGLRenderController() {
    if (_skyboxVao != 0) {
        glDeleteVertexArrays(1, &_skyboxVao);
        glDeleteBuffers(1, &_skyboxVbo);
    }
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

void OpenGLRenderController::Render(const std::shared_ptr<RenderRepository>& repository) noexcept {
    const auto renderData = repository->GetData();

    // Render skybox first (before any scene geometry)
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
}
