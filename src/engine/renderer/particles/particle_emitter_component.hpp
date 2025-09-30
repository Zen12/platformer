#pragma once
#include "../../entity/entity.hpp"
#include "glm/vec3.hpp"
#include "../mesh/mesh.hpp"
#include "../material/material.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include "../../utlis/engine_math.hpp"
#include "../transform/transform_component.hpp"

struct ParticleData {
public:
    size_t Count;
    float Duration;
    glm::vec3 StartVelocity;
    glm::vec3 EndVelocity;
    float StartScale;
    float EndScale;
    glm::vec4 StartColor;
    glm::vec4 EndColor;
};

class ParticleSystem {
public:
    bool RequestToRemove{false};

private:
    std::vector<std::unique_ptr<Mesh>> _particles;
    const ParticleData _data;
    glm::vec3 _currentPosition;
    glm::vec3 _currentScale{};
    glm::vec3 _velocity{};
    glm::vec4 _currentColor{};
    float _duration{0};
    std::weak_ptr<Material> _material;

public:
    explicit ParticleSystem(
        const ParticleData &data,
        const glm::vec3 &currentPosition,
        const std::weak_ptr<Material> &material)
        : _data(data), _currentPosition(currentPosition), _material(material) {
        for (size_t i = 0; i < _data.Count; i++) {

            _velocity = glm::mix(_data.StartVelocity, _data.EndVelocity, EngineMath::RandomFloat());
            _particles.push_back(Mesh::GenerateSpritePtr());
            _currentScale = glm::vec3(_data.StartScale);
            _currentColor = _data.StartColor;
        }
    }

    void Update(const float& deltaTime);

    void Render() const;
};

class ParticleEmitterComponent final : public Component {
private:
    ParticleData _data{};
    std::weak_ptr<Material> _material;

    std::vector<std::unique_ptr<ParticleSystem>> _particleSystems;

public:
    explicit ParticleEmitterComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Emit(const glm::vec3 &position) noexcept {
        if (const auto entity = _entity.lock()) {
            for (size_t i = 0; i < _data.Count; i++) {
                _particleSystems.emplace_back(std::make_unique<ParticleSystem>(_data, position, _material));
            }
        }
    }

    void SetMaterial(const std::weak_ptr<Material> &material) noexcept {
        _material = material;
    }

    void SetUniformMat4(const std::string &value, const glm::mat4 &mat) const noexcept;

    void SetData(const ParticleData &data) noexcept {
        _data = data;
    }

    void OverrideDataVelocity(const glm::vec3 &startVelocity, const glm::vec3 &endVelocity) noexcept {
        _data.StartVelocity = startVelocity;
        _data.EndVelocity = endVelocity;
    }

    void Update(const float &deltaTime) override;

    void Render() const;

private:
    void RemoveRequestedParticles() noexcept;
};