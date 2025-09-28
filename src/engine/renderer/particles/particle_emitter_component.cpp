#include "particle_emitter_component.hpp"

void ParticleSystem::Update(const float &deltaTime) {
    _duration += deltaTime;
    if (_duration >= _data.Duration) {
        RequestToRemove = true;
        return;
    }

    const auto lerpTime = EngineMath::InverseLerp(0.0f, _data.Duration, _duration);
    const auto currentScale = EngineMath::Lerp(_data.StartScale, _data.EndScale, lerpTime);
    _currentScale = glm::vec3(currentScale);
    _currentPosition += _velocity * deltaTime;
    _currentColor = glm::mix(_data.StartColor, _data.EndColor, lerpTime);
}


void ParticleSystem::Render() const {
    if (const auto material = _material.lock()) {
        const auto model = glm::scale(glm::translate(glm::mat4(1.0f), _currentPosition), _currentScale);
        material->SetMat4("model", model);
        material->SetVec4("color", _currentColor);
        material->Bind();
        for (const auto& mesh: _particles) {
            glDrawElements(GL_TRIANGLES, static_cast<int32_t>(mesh->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
        }
    }
}

void ParticleEmitterComponent::SetUniformMat4(const std::string &value, const glm::mat4 &mat) const noexcept {
    if (const auto material = _material.lock()) {
        material->SetMat4(value, mat);
    }
}

void ParticleEmitterComponent::Update(const float &deltaTime) {
    for (const auto &particles: _particleSystems) {
        particles->Update(deltaTime);
    }
    RemoveRequestedParticles();
}

void ParticleEmitterComponent::Render() const {
    for (const auto &particles: _particleSystems) {
        particles->Render();
    }
}

void ParticleEmitterComponent::RemoveRequestedParticles() noexcept {
    _particleSystems.erase(
        std::remove_if(
            _particleSystems.begin(),
            _particleSystems.end(),
            [](const std::unique_ptr<ParticleSystem> &particles) {
                return particles->RequestToRemove;
            }
        ),
        _particleSystems.end()
    );
}