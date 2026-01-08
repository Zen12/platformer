#pragma once
#include "particle_emitter_component.hpp"
#include "../esc_core.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../scene/scene.hpp"
#include "../camera/camera_component.hpp"
#include "../transform/transform_component.hpp"
#include <glm/gtc/matrix_transform.hpp>

class ParticleRenderSystem final : public ISystemView<ParticleEmitterComponent, TransformComponentV2> {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    const std::shared_ptr<RenderRepository> _repository;
    const std::weak_ptr<Scene> _scene;

public:
    explicit ParticleRenderSystem(
        const TypeView& view,
        const TypeCamera& camera,
        const std::shared_ptr<RenderRepository>& repository,
        const std::weak_ptr<Scene>& scene)
        : ISystemView(view), _cameraView(camera), _repository(repository), _scene(scene) {}

    void OnTick() override {
        for (const auto& [_, camera] : _cameraView.each()) {
            for (const auto& [__, emitter, transform] : View.each()) {
                RenderEmitter(emitter, camera);
            }
        }
    }

private:
    void RenderEmitter(const ParticleEmitterComponent& emitter, const CameraComponentV2& camera) const {
        const auto& particles = emitter.GetParticles();
        const std::string& materialGuid = emitter.GetMaterialGuid();
        const std::string& meshGuid = emitter.GetMeshGuid();
        const float particleSize = emitter.GetParticleSize();

        if (materialGuid.empty() || meshGuid.empty()) {
            return;
        }

        for (const auto& particle : particles) {
            if (!particle.Active) continue;

            const glm::mat4 model = GetBillboardMatrix(particle.Position, camera.View, particleSize);

            _repository->Add(RenderData{
                materialGuid,
                meshGuid,
                model,
                camera.View,
                camera.Projection,
                std::nullopt,  // BoneTransforms
                PrimitiveType::Triangles,
                std::nullopt,  // Positions
                std::nullopt,  // LineColor
                particle.Color  // InstanceColor
            });
        }
    }

    [[nodiscard]] static glm::mat4 GetBillboardMatrix(const glm::vec3& position, const glm::mat4& viewMatrix, float size) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);

        // Extract camera right and up vectors from view matrix
        const glm::vec3 right = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        const glm::vec3 up = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
        const glm::vec3 forward = glm::cross(right, up);

        // Build rotation matrix to face camera
        model[0] = glm::vec4(right * size, 0.0f);
        model[1] = glm::vec4(up * size, 0.0f);
        model[2] = glm::vec4(forward * size, 0.0f);

        return model;
    }
};
