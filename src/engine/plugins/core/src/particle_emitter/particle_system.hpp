#pragma once
#include "esc/esc_core.hpp"
#include "particle_emitter_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../health/health_component.hpp"
#include "plugins/ai/src/combat_state/combat_state_component.hpp"
#include <glm/glm.hpp>
#include <unordered_set>

class ParticleSystem final : public ISystemView<ParticleEmitterComponent, TransformComponentV2> {
private:
    using TypeLandingView = decltype(std::declval<entt::registry>().view<ParticleEmitterComponent, NavmeshAgentComponent>());
    using TypeDamageView = decltype(std::declval<entt::registry>().view<ParticleEmitterComponent, HealthComponent>());
    using TypeAttackView = decltype(std::declval<entt::registry>().view<ParticleEmitterComponent, CombatStateComponent>());

    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    TypeLandingView _landingView;
    TypeDamageView _damageView;
    TypeAttackView _attackView;
    std::unordered_set<entt::entity> _wasAttacking;

public:
    ParticleSystem(
        TypeView view,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView,
        TypeLandingView landingView,
        TypeDamageView damageView,
        TypeAttackView attackView)
        : ISystemView(view), _timeView(timeView), _landingView(landingView), _damageView(damageView), _attackView(attackView) {}

    void OnTick() override {
        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        if (deltaTime <= 0.0f) return;

        // Check for landing triggers
        for (auto [entity, emitter, agent] : _landingView.each()) {
            if (emitter.GetTriggerOnLand() && agent.JustLanded) {
                emitter.TriggerBurst(emitter.GetBurstCount());
            }
        }

        // Check for damage triggers
        for (auto [entity, emitter, health] : _damageView.each()) {
            if (emitter.GetTriggerOnDamage() && health.JustTookDamage) {
                emitter.TriggerBurst(emitter.GetBurstCount());
            }
        }

        // Check for attack triggers (trigger when attack starts)
        for (auto [entity, emitter, combat] : _attackView.each()) {
            if (emitter.GetTriggerOnAttack()) {
                const bool wasAttacking = _wasAttacking.count(entity) > 0;
                if (combat.IsAttacking && !wasAttacking) {
                    emitter.TriggerBurst(emitter.GetBurstCount());
                }
                if (combat.IsAttacking) {
                    _wasAttacking.insert(entity);
                } else {
                    _wasAttacking.erase(entity);
                }
            }
        }

        for (auto [entity, emitter, transform] : View.each()) {
            UpdateEmitter(emitter, transform, deltaTime);
        }
    }

private:
    static void UpdateEmitter(ParticleEmitterComponent& emitter, const TransformComponentV2& transform, float deltaTime) {
        const glm::vec3 emitterPos = transform.GetPosition();
        auto& particles = emitter.GetParticles();

        // Handle burst spawning
        const int burstCount = emitter.GetPendingBurst();
        if (burstCount > 0) {
            for (int i = 0; i < burstCount; ++i) {
                SpawnParticle(emitter, emitterPos);
            }
            emitter.ClearPendingBurst();
        }

        // Update existing particles
        for (auto& particle : particles) {
            if (!particle.Active) continue;

            // Apply gravity
            particle.Velocity += emitter.GetGravity() * deltaTime;

            // Update position
            particle.Position += particle.Velocity * deltaTime;

            // Update lifetime
            particle.Lifetime -= deltaTime;

            // Interpolate color
            const float t = 1.0f - (particle.Lifetime / particle.MaxLifetime);
            particle.Color = glm::mix(emitter.GetStartColor(), emitter.GetEndColor(), t);

            // Deactivate dead particles
            if (particle.Lifetime <= 0.0f) {
                particle.Active = false;
            }
        }

        // Spawn new particles
        const float emissionRate = emitter.GetEmissionRate();
        if (emissionRate <= 0.0f) return;

        float& accumulator = emitter.GetEmissionAccumulator();
        accumulator += deltaTime;

        const float spawnInterval = 1.0f / emissionRate;
        while (accumulator >= spawnInterval) {
            accumulator -= spawnInterval;
            SpawnParticle(emitter, emitterPos);
        }
    }

    static void SpawnParticle(ParticleEmitterComponent& emitter, const glm::vec3& emitterPos) {
        auto& particles = emitter.GetParticles();

        // Find inactive particle to reuse
        for (auto& particle : particles) {
            if (particle.Active) continue;

            // Initialize particle
            particle.Active = true;
            particle.Position = emitterPos + emitter.GetPositionOffset();

            // Random velocity
            const glm::vec3& baseVel = emitter.GetInitialVelocity();
            const glm::vec3& variance = emitter.GetVelocityVariance();
            particle.Velocity = glm::vec3(
                baseVel.x + emitter.RandomFloat(-variance.x, variance.x),
                baseVel.y + emitter.RandomFloat(-variance.y, variance.y),
                baseVel.z + emitter.RandomFloat(-variance.z, variance.z)
            );

            // Random lifetime
            const float baseLifetime = emitter.GetParticleLifetime();
            const float lifeVariance = emitter.GetLifetimeVariance();
            particle.MaxLifetime = baseLifetime + emitter.RandomFloat(-lifeVariance, lifeVariance);
            particle.MaxLifetime = std::max(0.1f, particle.MaxLifetime);
            particle.Lifetime = particle.MaxLifetime;

            // Initial color
            particle.Color = emitter.GetStartColor();

            return; // Only spawn one particle per call
        }
    }
};
