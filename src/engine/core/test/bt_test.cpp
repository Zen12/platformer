#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "bt_status.hpp"
#include "bt_def.hpp"

TEST_CASE("BTStatus values") {
    CHECK(static_cast<int>(BTStatus::Success) == 0);
    CHECK(static_cast<int>(BTStatus::Failure) == 1);
    CHECK(static_cast<int>(BTStatus::Running) == 2);
}

TEST_CASE("BTNodeDef default construction") {
    BTNodeDef def;

    CHECK(def.Type.empty());
    CHECK(def.ChildIndices.empty());
}

TEST_CASE("BTNodeDef with values") {
    BTNodeDef def;
    def.Type = "Sequence";
    def.ChildCount = 3;
    def.FirstChildIndex = 1;
    def.ChildIndices = {1, 2, 3};

    CHECK(def.Type == "Sequence");
    CHECK(def.ChildCount == 3);
    CHECK(def.FirstChildIndex == 1);
    CHECK(def.ChildIndices.size() == 3);
    CHECK(def.ChildIndices[0] == 1);
    CHECK(def.ChildIndices[1] == 2);
    CHECK(def.ChildIndices[2] == 3);
}

TEST_CASE("BehaviorTreeDef default construction") {
    BehaviorTreeDef def;

    CHECK(def.Nodes.empty());
    CHECK(def.Name.empty());
}

TEST_CASE("BehaviorTreeDef with nodes") {
    BehaviorTreeDef def;
    def.Name = "TestTree";

    BTNodeDef root;
    root.Type = "Selector";
    root.ChildCount = 2;
    root.FirstChildIndex = 1;

    BTNodeDef child1;
    child1.Type = "Action";

    BTNodeDef child2;
    child2.Type = "Action";

    def.Nodes.push_back(root);
    def.Nodes.push_back(child1);
    def.Nodes.push_back(child2);

    CHECK(def.Nodes.size() == 3);
    CHECK(def.GetRoot().Type == "Selector");
    CHECK(def.GetRoot().ChildCount == 2);
    CHECK(def.GetNode(1).Type == "Action");
}

TEST_CASE("BTExecutionState operations") {
    BTExecutionState state;

    SUBCASE("Reset initializes stack") {
        state.Reset();
        CHECK(state.StackDepth == 1);
        CHECK(state.Current().NodeIndex == 0);
        CHECK(state.Current().LastStatus == BTStatus::Running);
    }

    SUBCASE("Push adds to stack") {
        state.Reset();
        state.Push(5);
        CHECK(state.StackDepth == 2);
        CHECK(state.Current().NodeIndex == 5);
    }

    SUBCASE("Pop removes from stack") {
        state.Reset();
        state.Push(5);
        state.Pop();
        CHECK(state.StackDepth == 1);
        CHECK(state.Current().NodeIndex == 0);
    }
}
