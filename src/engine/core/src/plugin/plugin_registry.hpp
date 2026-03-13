#pragma once

#include "../register/fsm_action_registry.hpp"
#include "../register/fsm_condition_registry.hpp"
#include "../register/esc_system_registry.hpp"
#include "entity/component_registry.hpp"

class ResourceFactory;

struct PluginRegistries {
    FsmActionRegistry& Actions;
    FsmConditionRegistry& Conditions;
    ComponentRegistry& Components;
    EscSystemRegistry& Systems;
    ResourceFactory& Resources;
};
