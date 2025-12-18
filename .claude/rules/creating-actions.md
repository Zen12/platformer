# Creating FSM Actions

This guide explains how to create new actions for the Finite State Machine (FSM) system.

## Overview

Actions are behaviors that execute when the FSM enters, updates, or exits a state. They follow a three-file pattern similar to components, with serialization support for YAML configuration.

## Action Lifecycle

Actions have three lifecycle methods:
- **OnEnter()** - Called once when the FSM enters the state
- **OnUpdate()** - Called every frame while in the state
- **OnExit()** - Called once when the FSM exits the state

## File Structure

Each action requires **three files**:

1. **Action Implementation** (`*_action.hpp`)
   - Inherits from `Action` base class
   - Implements lifecycle methods
   - Stores dependencies as member variables

2. **Action Serialization** (`*_action_serialization.hpp`)
   - Inherits from `ActionSerialization`
   - Contains serializable configuration fields
   - Empty struct if no configuration needed

3. **YAML Serialization** (`*_action_serialization_yaml.hpp`)
   - YAML template specialization
   - Maps YAML fields to serialization struct
   - Empty decode if no fields to deserialize

## Step-by-Step Guide

### 1. Create Action Serialization Header

**Location:** `src/engine/fsm/node/action/<action_name>_action_serialization.hpp`

```cpp
#pragma once
#include <string>
#include "action_serialiazion.hpp"

class MyActionSerialization final : public ActionSerialization {
public:
    // Add serializable fields here
    std::string SomeParameter;
    float SomeValue;

    ~MyActionSerialization() override = default;
};
```

**Note:** Use the existing misspelled filename `action_serialiazion.hpp` for consistency.

### 2. Create YAML Serialization Header

**Location:** `src/engine/fsm/node/action/<action_name>_action_serialization_yaml.hpp`

```cpp
#pragma once
#include "<action_name>_action_serialization.hpp"
#include "yaml-cpp/node/node.h"

namespace YAML {
    template <>
    struct convert<MyActionSerialization> {
        static bool decode(const Node &node, MyActionSerialization &rhs) {
            rhs.SomeParameter = node["some_parameter"].as<std::string>();
            rhs.SomeValue = node["some_value"].as<float>();
            return true;
        }
    };
}
```

### 3. Create Action Implementation Header

**Location:** `src/engine/fsm/node/action/<action_name>_action.hpp`

```cpp
#pragma once
#include "action.hpp"
#include <memory>

struct MyAction final : public Action {
private:
    // Dependencies (passed by ActionFactory)
    std::shared_ptr<SomeManager> _manager;
    std::string _parameter;
    float _value;

public:
    MyAction(const std::string& parameter, float value, std::shared_ptr<SomeManager> manager)
        : _parameter(parameter), _value(value), _manager(std::move(manager)) {}

    void OnEnter() const override {
        // Initialize when entering state
    }

    void OnUpdate() const override {
        // Update every frame
    }

    void OnExit() const override {
        // Cleanup when exiting state
    }
};
```

### 4. Update ActionFactory

**File:** `src/engine/fsm/node/action/action_factory.hpp`

#### Add Include
```cpp
#include "<action_name>_action.hpp"
```

#### Add Private Members (if needed)
```cpp
private:
    std::shared_ptr<SomeManager> _someManager;
```

#### Update Constructor (if adding new dependencies)
```cpp
ActionFactory(std::shared_ptr<UIManager> uiManager,
              std::shared_ptr<SceneManager> sceneManager,
              std::unordered_map<std::string, bool>& triggers,
              std::unordered_map<SystemTriggers, bool>& systemTriggers,
              std::shared_ptr<SomeManager> someManager)  // Add new parameter
    : _uiManager(std::move(uiManager)),
      _sceneManager(std::move(sceneManager)),
      _triggers(triggers),
      _systemTriggers(systemTriggers),
      _someManager(std::move(someManager)) {}  // Add initialization
```

#### Add Factory Method
```cpp
[[nodiscard]] MyAction CreateMyAction(const std::string& parameter, float value) const {
    return {parameter, value, _someManager};
}
```

### 5. Update FsmController

**File:** `src/engine/fsm/fsm_controller.hpp`

#### Add Include
```cpp
#include "node/action/<action_name>_action.hpp"
```

#### Update ActionFactory Creation (if adding dependencies)
```cpp
ActionFactory actionFactory(uiManager, sceneManager, _triggers, _systemTrigger, someManager);
```

#### Add Action Type Branch
In the constructor, add a new branch in the action creation loop:

```cpp
} else if (actionData.Type == "my_action_type") {
    actions.emplace_back(actionFactory.CreateMyAction(actionData.Param, std::stof(actionData.Param2)));
```

**Note:** Use `actionData.Param` and `actionData.Param2` for simple parameter passing.

### 6. Update StateNode Variant

**File:** `src/engine/fsm/node/node.hpp`

#### Add Include
```cpp
#include "action/<action_name>_action.hpp"
```

#### Update AllActionVariants
```cpp
using AllActionVariants = std::variant<
    UiPageAction,
    LoadSceneAction,
    ButtonListenerAction,
    TriggerSetterButtonListenerAction,
    SetSystemTriggerAction,
    MyAction  // Add your action here
>;
```

### 7. Update YAML Deserialization

**File:** `src/engine/fsm/node/node_serialization_yaml.hpp`

#### Add Includes
```cpp
#include "action/<action_name>_action.hpp"
#include "action/<action_name>_action_serialization.hpp"
#include "action/<action_name>_action_serialization_yaml.hpp"
```

#### Add Decode Branch
In the `DecodeAction` method:

```cpp
} else if (type == "my_action_type") {
    const auto serialization = node.as<MyActionSerialization>();
    data.Param = serialization.SomeParameter;
    data.Param2 = std::to_string(serialization.SomeValue);
```

### 8. Build and Test

Always build after making changes:
```bash
./run_debug.sh
```

## YAML Configuration Example

```yaml
nodes:
- name: "My State"
  guid: "myStateGuid"
  actions:
    - type: my_action_type
      some_parameter: "example"
      some_value: 1.5
```

## Common Patterns

### Passing References (e.g., Trigger Maps)

If your action needs to modify a map or collection managed by FsmController:

1. Add reference member to ActionFactory:
```cpp
std::unordered_map<SystemTriggers, bool>& _systemTriggers;
```

2. Pass reference in constructor:
```cpp
ActionFactory(..., std::unordered_map<SystemTriggers, bool>& systemTriggers)
    : ..., _systemTriggers(systemTriggers) {}
```

3. Action receives reference:
```cpp
struct MyAction final : public Action {
private:
    std::unordered_map<SystemTriggers, bool>& _triggers;
public:
    MyAction(std::unordered_map<SystemTriggers, bool>& triggers)
        : _triggers(triggers) {}
};
```

### Using Enums

If your action uses enums, create a separate header to avoid circular dependencies:

```cpp
// system_triggers.hpp
#pragma once
#include <cstddef>
#include <functional>

enum class MyEnum {
    Value1 = 0,
    Value2 = 1,
};

// Add hash specialization for use in unordered_map
namespace std {
    template <>
    struct hash<MyEnum> {
        std::size_t operator()(const MyEnum& e) const noexcept {
            return std::hash<int>{}(static_cast<int>(e));
        }
    };
}
```

### Empty Actions (No Configuration)

If your action has no serializable parameters:

```cpp
// Serialization - empty struct
struct MyActionSerialization final : public ActionSerialization {
    ~MyActionSerialization() override = default;
};

// YAML - empty decode
namespace YAML {
    template <>
    struct convert<MyActionSerialization> {
        static bool decode([[maybe_unused]] const Node &node,
                         [[maybe_unused]] MyActionSerialization &rhs) {
            return true;
        }
    };
}
```

## Checklist

Before completing your action implementation, verify:

- [ ] Created three action files (action, serialization, yaml)
- [ ] Added action to ActionFactory (include, member if needed, factory method)
- [ ] Updated FsmController (include, factory creation if needed, action branch)
- [ ] Updated StateNode variant type
- [ ] Updated YAML deserialization (includes, decode branch)
- [ ] Built successfully with `./run_debug.sh`
- [ ] Tested action in FSM YAML file
- [ ] No compiler warnings

## Existing Actions Reference

### SetSystemTriggerAction
**Purpose:** Sets system triggers (Exit/Reload) when state is entered
**Location:** `src/engine/fsm/node/action/set_system_trigger_action.hpp`
**Pattern:** Takes map reference, uses enum with hash specialization

### TriggerSetterButtonListenerAction
**Purpose:** Registers button click handler that sets a trigger
**Location:** `src/engine/renderer/ui/trigger_setter_button_listener_action.hpp`
**Pattern:** Uses OnEnter/OnExit for setup/cleanup, stores handler ID

### LoadSceneAction
**Purpose:** Loads a scene by GUID
**Location:** `src/engine/scene/load_scene_action.hpp`
**Pattern:** Simple OnEnter action with manager dependency

### UiPageAction
**Purpose:** Loads/unloads UI page
**Location:** `src/engine/renderer/ui/ui_page_action.hpp`
**Pattern:** OnEnter loads, OnExit unloads

## Tips

- Keep actions focused on a single responsibility
- Use OnEnter for one-time initialization
- Use OnUpdate for continuous behavior
- Use OnExit for cleanup
- Prefer const methods when possible
- Minimize allocations in OnUpdate (called every frame)
- Use references for shared state (triggers, maps)
- Use shared_ptr for manager dependencies
- Extract enums to separate headers to avoid circular dependencies