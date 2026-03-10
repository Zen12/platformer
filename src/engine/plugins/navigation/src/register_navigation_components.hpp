#pragma once

#include "entity/component_registry.hpp"

#include "navmesh_agent/navmesh_agent_component.hpp"
#include "navmesh_agent/navmesh_agent_component_serialization.hpp"
#include "navmesh_agent/navmesh_agent_component_serialization_yaml.hpp"

inline void RegisterNavigationComponents(ComponentRegistry& registry) {
    // navmesh_agent
    registry.Register<NavmeshAgentComponentSerialization, NavmeshAgentComponent>("navmesh_agent",
        [](const NavmeshAgentComponentSerialization& s) {
            return NavmeshAgentComponent(s);
        }
    );
}
