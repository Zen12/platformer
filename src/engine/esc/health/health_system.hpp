#pragma once
#include "health_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../tag/tag_component.hpp"
#include "../esc_core.hpp"
#include "../combat_state/combat_state_component.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>

#define DEBUG_HEALTH 0

#if DEBUG_HEALTH
#define HEALTH_LOG(msg) std::cout << msg << std::endl
#else
#define HEALTH_LOG(msg)
#endif

class HealthSystem final : public ISystemView<HealthComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    using TypeCombatState = decltype(std::declval<entt::registry>().view<CombatStateComponent, TransformComponentV2>());

    const TypeDeltaTime _deltaTimeView;
    const TypeCombatState _btView;

    static constexpr float DAMAGE_PER_ATTACK = 10.0f;
    static constexpr float ATTACK_RANGE = 1.5f;
    static constexpr float DAMAGE_COOLDOWN = 1.0f;

    std::unordered_map<entt::entity, float> _damageCooldowns;

public:
    HealthSystem(
        const TypeView& view,
        const TypeDeltaTime& deltaTimeView,
        const TypeCombatState& btView)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _btView(btView) {}

    void OnTick() override {
        float deltaTime = 0.0f;
        for (auto [entity, time] : _deltaTimeView.each()) {
            deltaTime = time.Delta;
        }

        for (auto& [entity, cooldown] : _damageCooldowns) {
            cooldown -= deltaTime;
        }

        for (auto [entity, health, transform] : View.each()) {
            // Clear damage flag from previous frame
            health.JustTookDamage = false;

            if (health.IsDead()) continue;

            glm::vec3 playerPos = transform.GetPosition();

            for (auto [btEntity, bt, btTransform] : _btView.each()) {
                if (!bt.IsAttacking) continue;

                if (_damageCooldowns[btEntity] > 0.0f) continue;

                glm::vec3 zombiePos = btTransform.GetPosition();
                float distance = glm::distance(playerPos, zombiePos);

                if (distance <= ATTACK_RANGE) {
                    health.TakeDamage(DAMAGE_PER_ATTACK);
                    health.JustTookDamage = true;
                    _damageCooldowns[btEntity] = DAMAGE_COOLDOWN;

                    HEALTH_LOG("Player took " << DAMAGE_PER_ATTACK << " damage! HP: "
                              << health.GetCurrentHealth() << "/" << health.GetMaxHealth());

                    if (health.IsDead()) {
                        HEALTH_LOG("=== PLAYER DIED ===");
                    }
                }
            }
        }
    }
};
