#pragma once

#include "entity/component_registry.hpp"

#include "player_controller/player_controller_component.hpp"
#include "player_controller/player_controller_component_serialization.hpp"
#include "player_controller/player_controller_component_serialization_yaml.hpp"

#include "spawner/spawner_component.hpp"
#include "spawner/spawner_component_serialization.hpp"
#include "spawner/spawner_component_serialization_yaml.hpp"

#include "health/health_component.hpp"
#include "health/health_component_serialization.hpp"
#include "health/health_component_serialization_yaml.hpp"

inline void RegisterGameComponents(ComponentRegistry& registry) {
    // player_controller
    registry.Register<PlayerControllerComponentSerialization, PlayerControllerComponent>("player_controller",
        [](const PlayerControllerComponentSerialization& s) {
            return PlayerControllerComponent(s.MoveSpeed, s.DestinationDistance);
        }
    );

    // spawner
    registry.Register<SpawnerComponentSerialization, SpawnerComponent>("spawner",
        [](const SpawnerComponentSerialization& s) {
            return SpawnerComponent(s.PrefabGuid, s.SpawnCount, s.SpawnPositions, s.SpawnOnNavmesh, s.SpawnOnAllCells, s.ElevationHeight, s.YOffset);
        }
    );

    // health
    registry.Register<HealthComponentSerialization, HealthComponent>("health",
        [](const HealthComponentSerialization& s) {
            return HealthComponent(s.MaxHealth);
        }
    );
}
