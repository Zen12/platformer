#pragma once

#include "entity/component_registry.hpp"

#include "bt_component.hpp"
#include "bt_component_serialization.hpp"
#include "bt_component_serialization_yaml.hpp"

#include "esc/combat_state/combat_state_component.hpp"

inline void RegisterAIComponents(ComponentRegistry& registry) {
    registry.Register("behavior_tree",
        [](const YAML::Node& n) -> std::unique_ptr<ComponentSerialization> {
            auto s = n.as<BehaviorTreeComponentSerialization>();
            return std::make_unique<BehaviorTreeComponentSerialization>(std::move(s));
        },
        [](entt::registry& reg, entt::entity entity, const ComponentSerialization* ser) {
            const auto* s = dynamic_cast<const BehaviorTreeComponentSerialization*>(ser);
            if (!s) return;
            reg.emplace<BehaviorTreeComponent>(entity, *s);
            // Also add CombatStateComponent so other systems can read combat state
            if (!reg.all_of<CombatStateComponent>(entity)) {
                reg.emplace<CombatStateComponent>(entity);
            }
        }
    );
}
