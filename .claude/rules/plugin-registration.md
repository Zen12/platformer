# Plugin Registration API

## Overview

Plugins register actions, conditions, components, and systems via `PluginRegistries`. Each registry offers **template overloads** that eliminate boilerplate. Always prefer the template APIs over the raw three-argument forms.

Reference implementation: `src/engine/examples/plugin_example/example_plugin.cpp`

## Plugin Structure

The plugin entry point (the file with the `Register` struct and `REGISTER_PLUGIN` macro) lives at the **plugin root**, not inside `src/`. Internal implementation files stay in `src/`.

```
my_plugin/
├── my_plugin.cpp          ← Plugin entry point (Register struct + REGISTER_PLUGIN)
├── CMakeLists.txt
├── src/                   ← Internal implementation
│   ├── component/
│   ├── system/
│   └── action/
└── test/
```

Local includes from the entry point use the `src/` prefix. External includes (from linked libraries) do not:

```cpp
#include "core/src/plugin/plugin_registrar.hpp"  // external (engine-core)

#include "src/component/my_component.hpp"                    // local
#include "src/component/my_component_serialization.hpp"      // local
#include "src/component/my_component_serialization_yaml.hpp" // local

struct MyPlugin {
    static void Register(PluginRegistries& registries) {
        // register actions, conditions, components, systems here
    }
};

REGISTER_PLUGIN(MyPlugin)
```

## FSM Actions

**Registry:** `registries.Actions` (`FsmActionRegistry`)

**Template API:** `Register<TSerialization>(typeName, builder)`
- `TSerialization` must inherit from `ActionSerialization`
- YAML deserialization is auto-generated
- Builder receives `(const TSerialization& s, const EngineContext& ctx)` and returns `std::unique_ptr<Action>`

```cpp
registries.Actions.Register<MyActionSerialization>("my_action",
    [](const MyActionSerialization& s, const EngineContext& ctx) {
        return std::make_unique<MyAction>(s.SomeParam);
    }
);
```

## FSM Conditions

**Registry:** `registries.Conditions` (`FsmConditionRegistry`)

**Template API:** `Register<TSerialization>(typeName, builder)`
- `TSerialization` must inherit from `ConditionSerialization`
- YAML deserialization is auto-generated
- **`Type` and `Guid` are auto-copied** from serialization to the built condition — do NOT set them manually
- Builder receives `(const TSerialization& s, const EngineContext& ctx)` and returns `std::unique_ptr<ConditionBase>`

```cpp
registries.Conditions.Register<MyConditionSerialization>("my_condition",
    [](const MyConditionSerialization& s, const EngineContext& ctx) {
        return std::make_unique<MyCondition>(s.Threshold);
    }
);
```

**Wrong** (unnecessary boilerplate):
```cpp
// DO NOT do this — Type/Guid are set automatically
auto cond = std::make_unique<MyCondition>();
cond->Type = s.Type;
cond->Guid = s.Guid;
return cond;
```

## ECS Components

**Registry:** `registries.Components` (`ComponentRegistry`)

### Converter API (preferred): `Register<TSerialization, TComponent>(typeName, converter)`
- `TSerialization` must inherit from `ComponentSerialization`
- YAML deserialization is auto-generated
- `reg.emplace<TComponent>(entity, ...)` is handled automatically
- Converter receives `(const TSerialization& s)` and returns `TComponent` by value

```cpp
registries.Components.Register<MyCompSerialization, MyComponent>("my_comp",
    [](const MyCompSerialization& s) {
        return MyComponent{s.Speed, s.Health};
    }
);
```

### Emplacer API (when you need registry/entity access): `Register<TSerialization>(typeName, emplacer)`
- Emplacer receives `(entt::registry& reg, entt::entity entity, const TSerialization& s)`
- Use only when the component needs other components or registry queries during construction

```cpp
registries.Components.Register<MyCompSerialization>("my_comp",
    [](entt::registry& reg, entt::entity entity, const MyCompSerialization& s) {
        auto& other = reg.get<OtherComponent>(entity);
        reg.emplace<MyComponent>(entity, s.Speed, other.Multiplier);
    }
);
```

**Choose converter when possible.** Use emplacer only when you need `reg` or `entity`.

## ECS Systems

**Registry:** `registries.Systems` (`EscSystemRegistry`)

**Template API:** `Register<TSystem>(typeName, factory, priority)`
- `TSystem` must inherit from `ISystem`
- Auto-upcasts the return — no `-> std::unique_ptr<ISystem>` needed
- Factory receives `(const EscSystemContext& ctx)` and returns `std::unique_ptr<TSystem>`
- Priority determines execution order (lower runs first)

```cpp
registries.Systems.Register<MySystem>("MySystem", [](const EscSystemContext& ctx) {
    return std::make_unique<MySystem>(*ctx.Registry);
}, 500);
```

**Wrong** (unnecessary return type annotation):
```cpp
// DO NOT do this — the template auto-upcasts
registries.Systems.Register("MySystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
    return std::make_unique<MySystem>(*ctx.Registry);
}, 500);
```

## Refactoring Checklist

When migrating existing registrations to plugin template APIs:

### Conditions
- [ ] Replace raw `Register(name, deserializer, builder)` with `Register<TSerialization>(name, builder)`
- [ ] Remove manual `cond->Type = s.Type; cond->Guid = s.Guid;` lines

### Components
- [ ] If emplacer just does `reg.emplace<T>(entity, T{...})`, switch to `Register<TSer, TComp>(name, converter)`
- [ ] If emplacer needs `reg` or `entity` for queries, use `Register<TSer>(name, emplacer)`

### Systems
- [ ] Replace raw `Register(name, builder, priority)` with `Register<TSystem>(name, factory, priority)`
- [ ] Remove `-> std::unique_ptr<ISystem>` return type annotations from lambdas

### All Types
- [ ] Build with `./run_debug.sh` after each change
- [ ] Verify no compiler warnings

## API Summary

| Registry | Template | Lambda signature | Returns |
|----------|----------|-----------------|---------|
| Actions | `Register<TSer>` | `(const TSer&, const EngineContext&)` | `unique_ptr<Action>` |
| Conditions | `Register<TSer>` | `(const TSer&, const EngineContext&)` | `unique_ptr<ConditionBase>` |
| Components | `Register<TSer, TComp>` | `(const TSer&)` | `TComp` by value |
| Components | `Register<TSer>` | `(registry&, entity, const TSer&)` | `void` |
| Systems | `Register<TSys>` | `(const EscSystemContext&)` | `unique_ptr<TSys>` |

## Testing

Reference test: `src/engine/examples/plugin_example/test/plugin_registration_test.cpp`

Tests use Doctest and verify registrations end-to-end: YAML deserialization, building, and emplacement.

### Running Tests

```bash
cmake -S . -B bin/debug/desktop -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build bin/debug/desktop --target engine-plugin-registration-test
bin/debug/desktop/src/engine/examples/plugin_example/test/engine-plugin-registration-test
```

### Test Structure

Each registry type has a test case that verifies:

1. **HasType** — registration was recorded
2. **Deserialize** — YAML round-trips through the serialization type
3. **Build/Emplace** — the built object has correct state

### Action Test Pattern

```cpp
class FsmActionRegistryTest : public ::testing::Test {
protected:
    FsmActionRegistry actions;

    void SetUp() override {
        actions.Register<MyActionSerialization>("my_action",
            [](const MyActionSerialization& s, const EngineContext& ctx) {
                return std::make_unique<MyAction>(s.Label);
            }
        );
    }
};

TEST_F(FsmActionRegistryTest, HasType) {
    EXPECT_TRUE(actions.HasType("my_action"));
}

TEST_F(FsmActionRegistryTest, DeserializeFromYaml) {
    YAML::Node node;
    node["label"] = "test";
    auto ser = actions.Deserialize("my_action", node);
    ASSERT_NE(ser, nullptr);
}

TEST_F(FsmActionRegistryTest, BuildFromSerialization) {
    YAML::Node node;
    node["label"] = "test";
    auto ser = actions.Deserialize("my_action", node);
    EngineContext ctx;
    auto action = actions.Build("my_action", ser.get(), ctx);
    ASSERT_NE(action, nullptr);
}
```

### Condition Test Pattern (verifies auto-copy of Type/Guid)

```cpp
class FsmConditionRegistryTest : public ::testing::Test {
protected:
    FsmConditionRegistry conditions;

    void SetUp() override {
        conditions.Register<MyConditionSerialization>("my_cond",
            [](const MyConditionSerialization& s, const EngineContext& ctx) {
                return std::make_unique<MyCondition>();
                // Type/Guid NOT set here — registry does it automatically
            }
        );
    }
};

TEST_F(FsmConditionRegistryTest, BuildAutoCopiesTypeAndGuid) {
    YAML::Node node;
    auto ser = conditions.Deserialize("my_cond", node);
    auto* typed = dynamic_cast<MyConditionSerialization*>(ser.get());
    typed->Type = "my_cond";
    typed->Guid = "test_guid";

    EngineContext ctx;
    auto cond = conditions.Build("my_cond", ser.get(), ctx);
    EXPECT_EQ(cond->Type, "my_cond");
    EXPECT_EQ(cond->Guid, "test_guid");
}
```

### Component Test Pattern (converter API)

```cpp
class ComponentRegistryConverterTest : public ::testing::Test {
protected:
    ComponentRegistry components;

    void SetUp() override {
        components.Register<MyCompSerialization, MyComponent>("my_comp",
            [](const MyCompSerialization& s) {
                return MyComponent{s.Value};
            }
        );
    }
};

TEST_F(ComponentRegistryConverterTest, EmplaceCreatesComponent) {
    YAML::Node node;
    node["value"] = 42;
    auto ser = components.Deserialize("my_comp", node);

    entt::registry reg;
    auto entity = reg.create();
    components.Emplace("my_comp", reg, entity, ser.get());

    EXPECT_TRUE(reg.all_of<MyComponent>(entity));
    EXPECT_EQ(reg.get<MyComponent>(entity).Value, 42);
}
```

### System Test Pattern

```cpp
class EscSystemRegistryTest : public ::testing::Test {
protected:
    EscSystemRegistry systems;

    void SetUp() override {
        systems.Register<MySystem>("MySystem", [](const EscSystemContext& ctx) {
            return std::make_unique<MySystem>(*ctx.Registry);
        }, 500);
    }
};

TEST_F(EscSystemRegistryTest, HasType) {
    EXPECT_TRUE(systems.HasType("MySystem"));
}

TEST_F(EscSystemRegistryTest, BuildCreatesSystem) {
    auto registry = std::make_shared<entt::registry>();
    EscSystemContext ctx{.Registry = registry};
    auto system = systems.Build("MySystem", ctx);
    ASSERT_NE(system, nullptr);
}
```

### Test CMakeLists Pattern

```cmake
# test/CMakeLists.txt
add_executable(engine-my-plugin-test
    my_plugin_test.cpp
)

target_include_directories(engine-my-plugin-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..   # src/engine — for core/src/register/, entity/, esc/
)

target_link_libraries(engine-my-plugin-test PRIVATE
    engine-my-plugin
    engine-core
    EnTT::EnTT
    GTest::gtest_main
)

add_test(NAME engine-my-plugin-test COMMAND engine-my-plugin-test)
```

Guarded in the parent CMakeLists.txt:
```cmake
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
```

### What to Test

- [ ] Each registered type deserializes from YAML correctly
- [ ] Each type builds/emplaces a valid object
- [ ] Condition Type/Guid are auto-copied (not set in builder)
- [ ] Component converter produces correct values
- [ ] System builds with a real `entt::registry`
- [ ] Unknown type returns nullptr / false

## Related Files

- `src/engine/core/src/register/fsm_action_registry.hpp`
- `src/engine/core/src/register/fsm_condition_registry.hpp`
- `src/engine/entity/component_registry.hpp`
- `src/engine/core/src/register/esc_system_registry.hpp`
- `src/engine/core/src/plugin/plugin_registrar.hpp`
- `src/engine/core/src/plugin/plugin_registry.hpp`
- `src/engine/examples/plugin_example/example_plugin.cpp`
- `src/engine/examples/plugin_example/test/plugin_registration_test.cpp`
