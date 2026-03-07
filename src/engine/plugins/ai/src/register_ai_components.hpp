#pragma once

#include "entity/component_registry.hpp"

#include "bt_component.hpp"
#include "bt_component_serialization.hpp"
#include "bt_component_serialization_yaml.hpp"

#include "combat_state/combat_state_component.hpp"
#include "combat_state/combat_state_component_serialization.hpp"
#include "combat_state/combat_state_component_serialization_yaml.hpp"

inline void RegisterAIComponents(ComponentRegistry& registry) {
    registry.Register<BehaviorTreeComponentSerialization>("behavior_tree",
        [](entt::registry& reg, entt::entity entity, const BehaviorTreeComponentSerialization& s) {
            reg.emplace<BehaviorTreeComponent>(entity, s);
            // Also add CombatStateComponent so other systems can read combat state
            if (!reg.all_of<CombatStateComponent>(entity)) {
                reg.emplace<CombatStateComponent>(entity);
            }
        }
    );

    registry.Register<CombatStateComponentSerialization, CombatStateComponent>("combat_state",
        []([[maybe_unused]] const CombatStateComponentSerialization& s) {
            return CombatStateComponent{};
        }
    );
}
