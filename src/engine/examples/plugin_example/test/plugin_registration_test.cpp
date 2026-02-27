#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <yaml-cpp/yaml.h>

#include "core/src/register/fsm_action_registry.hpp"
#include "core/src/register/fsm_condition_registry.hpp"
#include "core/src/register/esc_system_registry.hpp"
#include "entity/component_registry.hpp"
#include "esc/esc_system_context.hpp"

// Action
#include "action/example_noop_action.hpp"
#include "action/example_noop_action_serialization.hpp"
#include "action/example_noop_action_serialization_yaml.hpp"

// Condition
#include "condition/example_condition.hpp"
#include "condition/example_condition_serialization.hpp"
#include "condition/example_condition_serialization_yaml.hpp"

// Component
#include "component/example_component.hpp"
#include "component/example_component_serialization.hpp"
#include "component/example_component_serialization_yaml.hpp"

// System
#include "system/example_system.hpp"

// ============================================================
// FSM Action Registry
// ============================================================

TEST_CASE("FsmActionRegistry Register<TSerialization> template") {
    FsmActionRegistry actions;

    actions.Register<ExampleNoopActionSerialization>("example_noop",
        [](const ExampleNoopActionSerialization& s, [[maybe_unused]] const EngineContext& ctx) {
            return std::make_unique<ExampleNoopAction>(s.Label);
        }
    );

    CHECK(actions.HasType("example_noop"));
    CHECK_FALSE(actions.HasType("nonexistent"));

    SUBCASE("Deserialize from YAML") {
        YAML::Node node;
        node["type"] = "example_noop";
        node["label"] = "test_label";

        auto ser = actions.Deserialize("example_noop", node);
        REQUIRE(ser != nullptr);

        auto* typed = dynamic_cast<ExampleNoopActionSerialization*>(ser.get());
        REQUIRE(typed != nullptr);
        CHECK(typed->Label == "test_label");
    }

    SUBCASE("Build from serialization") {
        YAML::Node node;
        node["label"] = "built_label";

        auto ser = actions.Deserialize("example_noop", node);
        REQUIRE(ser != nullptr);

        EngineContext ctx;
        auto action = actions.Build("example_noop", ser.get(), ctx);
        REQUIRE(action != nullptr);
    }

    SUBCASE("Build unknown type returns nullptr") {
        EngineContext ctx;
        CHECK(actions.Build("nonexistent", nullptr, ctx) == nullptr);
    }
}

// ============================================================
// FSM Condition Registry
// ============================================================

TEST_CASE("FsmConditionRegistry Register<TSerialization> template") {
    FsmConditionRegistry conditions;

    conditions.Register<ExampleAlwaysPassConditionSerialization>("example_always_pass",
        []([[maybe_unused]] const ExampleAlwaysPassConditionSerialization& s,
           [[maybe_unused]] const EngineContext& ctx) {
            return std::make_unique<ExampleAlwaysPassCondition>();
        }
    );

    CHECK(conditions.HasType("example_always_pass"));

    SUBCASE("Deserialize from YAML") {
        YAML::Node node;
        node["type"] = "example_always_pass";
        node["guid"] = "cond_test_guid";

        auto ser = conditions.Deserialize("example_always_pass", node);
        REQUIRE(ser != nullptr);
    }

    SUBCASE("Build auto-copies Type and Guid") {
        YAML::Node node;
        node["type"] = "example_always_pass";
        node["guid"] = "cond_test_guid";

        auto ser = conditions.Deserialize("example_always_pass", node);
        REQUIRE(ser != nullptr);

        auto* typedSer = dynamic_cast<ExampleAlwaysPassConditionSerialization*>(ser.get());
        REQUIRE(typedSer != nullptr);
        typedSer->Type = "example_always_pass";
        typedSer->Guid = "cond_test_guid";

        EngineContext ctx;
        auto cond = conditions.Build("example_always_pass", ser.get(), ctx);
        REQUIRE(cond != nullptr);
        CHECK(cond->Type == "example_always_pass");
        CHECK(cond->Guid == "cond_test_guid");
        CHECK(cond->IsSuccess());
    }
}

// ============================================================
// Component Registry — Converter API
// ============================================================

TEST_CASE("ComponentRegistry Register<TSer, TComp> converter template") {
    ComponentRegistry components;

    components.Register<ExampleTagComponentSerialization, ExampleTagComponent>("example_tag",
        [](const ExampleTagComponentSerialization& s) {
            return ExampleTagComponent{s.Tag};
        }
    );

    CHECK(components.HasType("example_tag"));

    SUBCASE("Deserialize from YAML") {
        YAML::Node node;
        node["tag"] = "hello";

        auto ser = components.Deserialize("example_tag", node);
        REQUIRE(ser != nullptr);

        auto* typed = dynamic_cast<ExampleTagComponentSerialization*>(ser.get());
        REQUIRE(typed != nullptr);
        CHECK(typed->Tag == "hello");
    }

    SUBCASE("Emplace creates component on entity") {
        YAML::Node node;
        node["tag"] = "world";

        auto ser = components.Deserialize("example_tag", node);
        REQUIRE(ser != nullptr);

        entt::registry reg;
        auto entity = reg.create();

        bool ok = components.Emplace("example_tag", reg, entity, ser.get());
        CHECK(ok);
        CHECK(reg.all_of<ExampleTagComponent>(entity));
        CHECK(reg.get<ExampleTagComponent>(entity).Tag == "world");
    }
}

// ============================================================
// Component Registry — Emplacer API
// ============================================================

TEST_CASE("ComponentRegistry Register<TSer> emplacer template") {
    ComponentRegistry components;

    components.Register<ExampleTagComponentSerialization>("example_tag_emplacer",
        [](entt::registry& reg, entt::entity entity, const ExampleTagComponentSerialization& s) {
            reg.emplace<ExampleTagComponent>(entity, ExampleTagComponent{"prefix_" + s.Tag});
        }
    );

    SUBCASE("Emplace with custom logic") {
        YAML::Node node;
        node["tag"] = "test";

        auto ser = components.Deserialize("example_tag_emplacer", node);
        REQUIRE(ser != nullptr);

        entt::registry reg;
        auto entity = reg.create();

        components.Emplace("example_tag_emplacer", reg, entity, ser.get());
        CHECK(reg.all_of<ExampleTagComponent>(entity));
        CHECK(reg.get<ExampleTagComponent>(entity).Tag == "prefix_test");
    }
}

// ============================================================
// ECS System Registry
// ============================================================

TEST_CASE("EscSystemRegistry Register<TSystem> template") {
    EscSystemRegistry systems;

    systems.Register<ExampleSystem>("ExampleSystem", [](const EscSystemContext& ctx) {
        return std::make_unique<ExampleSystem>(*ctx.Registry);
    }, 900);

    CHECK(systems.HasType("ExampleSystem"));
    CHECK_FALSE(systems.HasType("nonexistent"));

    SUBCASE("Build creates system") {
        auto registry = std::make_shared<entt::registry>();
        EscSystemContext ctx;
        ctx.Registry = registry;

        auto system = systems.Build("ExampleSystem", ctx);
        REQUIRE(system != nullptr);
    }

    SUBCASE("BuildAll respects priority order") {
        auto registry = std::make_shared<entt::registry>();
        EscSystemContext ctx;
        ctx.Registry = registry;

        systems.Register<ExampleSystem>("EarlySystem", [](const EscSystemContext& c) {
            return std::make_unique<ExampleSystem>(*c.Registry);
        }, 100);

        auto all = systems.BuildAll(ctx);
        CHECK(all.size() == 2);
    }

    SUBCASE("Build unknown type returns nullptr") {
        EscSystemContext ctx;
        CHECK(systems.Build("nonexistent", ctx) == nullptr);
    }
}
