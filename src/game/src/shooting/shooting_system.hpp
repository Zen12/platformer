#pragma once
#include "player_controller/player_controller_component.hpp"
#include "transform/transform_component.hpp"
#include "navmesh_agent/navmesh_agent_component.hpp"
#include "plugins/ik/src/component/ik_aim_component.hpp"
#include "time/time_component.hpp"
#include "tag/tag_component.hpp"
#include "health/health_component.hpp"
#include "esc/esc_core.hpp"
#include "scene.hpp"
#include "input_system.hpp"
#include "navigation_manager.hpp"
#include "particle_emitter/particle_emitter_component.hpp"
#include "audio_manager.hpp"
#include "guid.hpp"
#include <glm/glm.hpp>
#include <memory>

class ShootingSystem final : public ISystemView<PlayerControllerComponent, TransformComponentV2, IKAimComponent, NavmeshAgentComponent> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    using TypeGunView = decltype(std::declval<entt::registry>().view<TagComponent, ParticleEmitterComponent>());

    const TypeDeltaTime _deltaTimeView;
    const TypeGunView _gunView;
    const std::weak_ptr<Scene> _scene;
    entt::registry& _registry;
    const std::shared_ptr<NavigationManager> _navigationManager;
    const std::weak_ptr<AudioManager> _audioManager;
    const Guid _shootSoundGuid = Guid::FromString("651c6f8a-a2eb-4cfc-b80a-11b81d04b732");

    float _shootCooldown = 0.0f;
    bool _lastShotRight = false;

    static constexpr float DAMAGE_PER_SHOT = 25.0f;

public:
    ShootingSystem(
        const TypeView& view,
        const TypeDeltaTime& deltaTimeView,
        const TypeGunView& gunView,
        const std::weak_ptr<Scene>& scene,
        entt::registry& registry,
        const std::shared_ptr<NavigationManager>& navigationManager,
        const std::weak_ptr<AudioManager>& audioManager)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _gunView(gunView),
          _scene(scene), _registry(registry), _navigationManager(navigationManager),
          _audioManager(audioManager) {}

    void OnTick() override {
        const auto scene = _scene.lock();
        if (!scene) return;

        const auto inputSystem = scene->GetInputSystem().lock();
        if (!inputSystem) return;

        if (!_navigationManager) return;

        float deltaTime = 0.0f;
        for (auto [entity, time] : _deltaTimeView.each()) {
            deltaTime = time.FixedDelta;
        }

        if (_shootCooldown > 0.0f) {
            _shootCooldown -= deltaTime;
        }

        if (!inputSystem->IsMousePress(MouseButton::Left)) return;

        for (auto [entity, controller, transform, ikAim, navAgent] : View.each()) {
            if (_shootCooldown > 0.0f) continue;
            if (!ikAim.HasAimTarget) continue;

            _shootCooldown = ikAim.RecoilDuration;

            if (const auto audio = _audioManager.lock()) {
                audio->PlaySoundSpatial(_shootSoundGuid, transform.GetPosition(), 0.9f, false, 0.0f, 30.0f);
            }

            // Muzzle flash alternating guns
            _lastShotRight = !_lastShotRight;
            const std::string targetTag = _lastShotRight ? "player_gun_R" : "player_gun_L";
            for (auto [gunEntity, gunTag, gunEmitter] : _gunView.each()) {
                if (gunTag.GetTag() == targetTag) {
                    gunEmitter.TriggerBurst(gunEmitter.GetBurstCount());
                    break;
                }
            }
            ikAim.RecoilTimer = ikAim.RecoilDuration;
            ikAim.RecoilHand = _lastShotRight ? 0 : 1;

            const glm::vec3 origin = transform.GetPosition();
            const glm::vec3& target = ikAim.AimTarget;

            // Navmesh raycast: traverses grid cells with elevation, checks for agents
            const ShootRaycastHit hit = _navigationManager->ShootRaycast(
                origin, target, navAgent.CurrentElevation, navAgent.CrowdAgentId);

            // Spawn impact particles at final position
            const glm::vec3 finalPos = hit.Hit ? hit.Point : target;
            auto* playerEmitter = _registry.try_get<ParticleEmitterComponent>(entity);
            if (playerEmitter) {
                playerEmitter->TriggerBurstAt(playerEmitter->GetBurstCount(), finalPos);
            }

            if (!hit.HitAgent) continue;

            // Resolve hit AgentId to entity
            entt::entity hitEntity = entt::null;
            const auto navView = _registry.view<NavmeshAgentComponent>();
            for (auto [navEntity, navComp] : navView.each()) {
                if (navComp.CrowdAgentId == hit.AgentId) {
                    hitEntity = navEntity;
                    break;
                }
            }

            if (hitEntity == entt::null) continue;

            // Check hit entity is a zombie with health
            const auto* tag = _registry.try_get<TagComponent>(hitEntity);
            if (!tag || tag->GetTag() != "zombie") continue;

            auto* health = _registry.try_get<HealthComponent>(hitEntity);
            if (!health) continue;
            if (health->IsDead()) continue;

            health->TakeDamage(DAMAGE_PER_SHOT);
            health->JustTookDamage = true;

            // Spawn blood particles at hit point
            auto* zombieEmitter = _registry.try_get<ParticleEmitterComponent>(hitEntity);
            if (zombieEmitter) {
                zombieEmitter->TriggerBurstAt(zombieEmitter->GetBurstCount(), hit.Point);
            }

            if (health->IsDead()) {
                auto* hitNavAgent = _registry.try_get<NavmeshAgentComponent>(hitEntity);
                if (hitNavAgent && hitNavAgent->CrowdAgentId >= 0) {
                    _navigationManager->GetCrowd()->RemoveAgent(hitNavAgent->CrowdAgentId);
                }
                _registry.destroy(hitEntity);
            }
        }
    }
};
