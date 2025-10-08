# Coding Style Rules

## General C++ Guidelines
- Use modern C++17 features where appropriate
- Prefer smart pointers over raw pointers
- Use const correctness consistently
- Use noexcept for simple setter methods
- Follow RAII principles for resource management

## Naming Conventions
- Use PascalCase for class names (e.g., `CharacterController`, `RenderObject`)
- Use PascalCase for method name (e.g., `UpdatePosition`)
- Use _camalCase for private variables (e.g., `_currentPosition`)
- Use ALL_CAPS for constants and enums

## Code Organization
- Keep header files (.hpp) and implementation files (.cpp) separate
- Group related functionality into classes

## Comments and Documentation
- Write clear, no comments

## Error Handling
- Use exceptions for exceptional cases
- Validate input parameters
- Handle edge cases explicitly

## Performance
- Minimize allocations in game loop
- Cache frequently accessed values
- Profile before optimizing

## Component Factory Pattern

### Overview
The project uses a factory pattern for component serialization and instantiation. Each component requires three files:

1. **Component Serialization** (`*_component_serialization.hpp`)
   - Inherits from `ComponentSerialization`
   - Contains serializable data members (configuration values)
   - Empty struct if component has no serializable data

2. **YAML Serialization** (`*_component_serialization_yaml.hpp`)
   - YAML template specialization for deserialization
   - Maps YAML nodes to serialization struct fields
   - Empty decode if no fields to deserialize

3. **Component Factory** (`*_component_factory.hpp`)
   - Inherits from `ComponentFactory<TComponent, TSerialization>`
   - Implements `FillComponent()` to initialize component
   - Resolves dependencies via `_scene` and `_entity` weak pointers

### Example Implementation

```cpp
// shoot_component_serialization.hpp
#pragma once
#include "../../../entity/component_serialization.hpp"

struct ShootComponentSerialization final : public ComponentSerialization {
    // Add serializable fields here (e.g., float FireRate;)
};

// shoot_component_serialization_yaml.hpp
#pragma once
#include <yaml-cpp/yaml.h>
#include "shoot_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<ShootComponentSerialization> {
        static bool decode([[maybe_unused]] const Node &node, [[maybe_unused]] ShootComponentSerialization &rhs) {
            // rhs.FireRate = node["fire_rate"].as<float>();
            return true;
        }
    };
}

// shoot_component_factory.hpp
#pragma once
#include "../../../entity/component_factory.hpp"
#include "shoot_component.hpp"
#include "shoot_component_serialization.hpp"

class ShootComponentFactory final : public ComponentFactory<ShootComponent, ShootComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ShootComponent> &component, const ShootComponentSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto entity = _entity.lock()) {
                    // Set component dependencies from scene/entity
                    comp->SetWorld(scene->GetPhysicsWorld());
                    comp->SetAnimation(entity->GetComponent<CharacterAnimationComponent>());
                    // Set serialized configuration values
                    // comp->SetFireRate(serialization.FireRate);
                }
            }
        }
    }
};
```

### Registration Steps

After creating the three files, register the component in two places:

1. **scene_manager.cpp**
   - Include factory and serialization headers
   - Add to `TryToAddComponents<>` template parameter list with pattern: `SerializationType, FactoryType`

2. **entity_serialization_yaml.hpp**
   - Include both serialization headers
   - Add entry to factories map with YAML type ID and Parse lambda

### Component Dependencies

Components can access:
- **Scene resources**: `_scene.lock()->GetPhysicsWorld()`, `GetRenderPipeline()`, etc.
- **Entity components**: `_entity.lock()->GetComponent<T>()`
- **Serialized data**: Values from the `serialization` parameter

## Build and Run
- Use `./run_debug.sh` to build and run the project in debug mode
- **CRITICAL**: ALWAYS run `./run_debug.sh` after ANY code change, even small ones
  - This applies to single-line changes, header modifications, YAML edits, etc.
  - No exceptions - every change must be verified with a build
  - This prevents broken builds and catches issues immediately
- Fix any compiler warnings before considering the task complete
- The build output will show compilation progress and any errors/warnings