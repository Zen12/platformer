#include <gtest/gtest.h>
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

class FsmActionRegistryTest : public ::testing::Test {
protected:
    FsmActionRegistry actions;

    void SetUp() override {
        actions.Register<ExampleNoopActionSerialization>("example_noop",
            [](const ExampleNoopActionSerialization& s, [[maybe_unused]] const EngineContext& ctx) {
                return std::make_unique<ExampleNoopAction>(s.Label);
            }
        );
    }
};

TEST_F(FsmActionRegistryTest, HasType) {
    EXPECT_TRUE(actions.HasType("example_noop"));
    EXPECT_FALSE(actions.HasType("nonexistent"));
}

TEST_F(FsmActionRegistryTest, DeserializeFromYaml) {
    YAML::Node node;
    node["type"] = "example_noop";
    node["label"] = "test_label";

    auto ser = actions.Deserialize("example_noop", node);
    ASSERT_NE(ser, nullptr);

    auto* typed = dynamic_cast<ExampleNoopActionSerialization*>(ser.get());
    ASSERT_NE(typed, nullptr);
    EXPECT_EQ(typed->Label, "test_label");
}

TEST_F(FsmActionRegistryTest, BuildFromSerialization) {
    YAML::Node node;
    node["label"] = "built_label";

    auto ser = actions.Deserialize("example_noop", node);
    ASSERT_NE(ser, nullptr);

    EngineContext ctx;
    auto action = actions.Build("example_noop", ser.get(), ctx);
    ASSERT_NE(action, nullptr);
}

TEST_F(FsmActionRegistryTest, BuildUnknownTypeReturnsNullptr) {
    EngineContext ctx;
    EXPECT_EQ(actions.Build("nonexistent", nullptr, ctx), nullptr);
}

// ============================================================
// FSM Condition Registry
// ============================================================

class FsmConditionRegistryTest : public ::testing::Test {
protected:
    FsmConditionRegistry conditions;

    void SetUp() override {
        conditions.Register<ExampleAlwaysPassConditionSerialization>("example_always_pass",
            []([[maybe_unused]] const ExampleAlwaysPassConditionSerialization& s,
               [[maybe_unused]] const EngineContext& ctx) {
                return std::make_unique<ExampleAlwaysPassCondition>();
            }
        );
    }
};

TEST_F(FsmConditionRegistryTest, HasType) {
    EXPECT_TRUE(conditions.HasType("example_always_pass"));
}

TEST_F(FsmConditionRegistryTest, DeserializeFromYaml) {
    YAML::Node node;
    node["type"] = "example_always_pass";
    node["guid"] = "cond_test_guid";

    auto ser = conditions.Deserialize("example_always_pass", node);
    ASSERT_NE(ser, nullptr);
}

TEST_F(FsmConditionRegistryTest, BuildAutoCopiesTypeAndGuid) {
    YAML::Node node;
    node["type"] = "example_always_pass";
    node["guid"] = "cond_test_guid";

    auto ser = conditions.Deserialize("example_always_pass", node);
    ASSERT_NE(ser, nullptr);

    auto* typedSer = dynamic_cast<ExampleAlwaysPassConditionSerialization*>(ser.get());
    ASSERT_NE(typedSer, nullptr);
    typedSer->Type = "example_always_pass";
    typedSer->Guid = "cond_test_guid";

    EngineContext ctx;
    auto cond = conditions.Build("example_always_pass", ser.get(), ctx);
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->Type, "example_always_pass");
    EXPECT_EQ(cond->Guid, "cond_test_guid");
    EXPECT_TRUE(cond->IsSuccess());
}

// ============================================================
// Component Registry -- Converter API
// ============================================================

class ComponentRegistryConverterTest : public ::testing::Test {
protected:
    ComponentRegistry components;

    void SetUp() override {
        components.Register<ExampleTagComponentSerialization, ExampleTagComponent>("example_tag",
            [](const ExampleTagComponentSerialization& s) {
                return ExampleTagComponent{s.Tag};
            }
        );
    }
};

TEST_F(ComponentRegistryConverterTest, HasType) {
    EXPECT_TRUE(components.HasType("example_tag"));
}

TEST_F(ComponentRegistryConverterTest, DeserializeFromYaml) {
    YAML::Node node;
    node["tag"] = "hello";

    auto ser = components.Deserialize("example_tag", node);
    ASSERT_NE(ser, nullptr);

    auto* typed = dynamic_cast<ExampleTagComponentSerialization*>(ser.get());
    ASSERT_NE(typed, nullptr);
    EXPECT_EQ(typed->Tag, "hello");
}

TEST_F(ComponentRegistryConverterTest, EmplaceCreatesComponentOnEntity) {
    YAML::Node node;
    node["tag"] = "world";

    auto ser = components.Deserialize("example_tag", node);
    ASSERT_NE(ser, nullptr);

    entt::registry reg;
    auto entity = reg.create();

    bool ok = components.Emplace("example_tag", reg, entity, ser.get());
    EXPECT_TRUE(ok);
    EXPECT_TRUE(reg.all_of<ExampleTagComponent>(entity));
    EXPECT_EQ(reg.get<ExampleTagComponent>(entity).Tag, "world");
}

// ============================================================
// Component Registry -- Emplacer API
// ============================================================

TEST(ComponentRegistryEmplacer, EmplaceWithCustomLogic) {
    ComponentRegistry components;

    components.Register<ExampleTagComponentSerialization>("example_tag_emplacer",
        [](entt::registry& reg, entt::entity entity, const ExampleTagComponentSerialization& s) {
            reg.emplace<ExampleTagComponent>(entity, ExampleTagComponent{"prefix_" + s.Tag});
        }
    );

    YAML::Node node;
    node["tag"] = "test";

    auto ser = components.Deserialize("example_tag_emplacer", node);
    ASSERT_NE(ser, nullptr);

    entt::registry reg;
    auto entity = reg.create();

    components.Emplace("example_tag_emplacer", reg, entity, ser.get());
    EXPECT_TRUE(reg.all_of<ExampleTagComponent>(entity));
    EXPECT_EQ(reg.get<ExampleTagComponent>(entity).Tag, "prefix_test");
}

// ============================================================
// ECS System Registry
// ============================================================

class EscSystemRegistryTest : public ::testing::Test {
protected:
    EscSystemRegistry systems;

    void SetUp() override {
        systems.Register<ExampleSystem>("ExampleSystem", [](const EscSystemContext& ctx) {
            return std::make_unique<ExampleSystem>(*ctx.Registry);
        }, 900);
    }
};

TEST_F(EscSystemRegistryTest, HasType) {
    EXPECT_TRUE(systems.HasType("ExampleSystem"));
    EXPECT_FALSE(systems.HasType("nonexistent"));
}

TEST_F(EscSystemRegistryTest, BuildCreatesSystem) {
    auto registry = std::make_shared<entt::registry>();
    EscSystemContext ctx;
    ctx.Registry = registry;

    auto system = systems.Build("ExampleSystem", ctx);
    ASSERT_NE(system, nullptr);
}

TEST_F(EscSystemRegistryTest, BuildAllRespectsPriorityOrder) {
    auto registry = std::make_shared<entt::registry>();
    EscSystemContext ctx;
    ctx.Registry = registry;

    systems.Register<ExampleSystem>("EarlySystem", [](const EscSystemContext& c) {
        return std::make_unique<ExampleSystem>(*c.Registry);
    }, 100);

    auto all = systems.BuildAll(ctx);
    EXPECT_EQ(all.size(), 2);
}

TEST_F(EscSystemRegistryTest, BuildUnknownTypeReturnsNullptr) {
    EscSystemContext ctx;
    EXPECT_EQ(systems.Build("nonexistent", ctx), nullptr);
}
