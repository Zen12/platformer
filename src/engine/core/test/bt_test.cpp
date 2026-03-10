#include <gtest/gtest.h>
#include "bt_status.hpp"
#include "bt_def.hpp"

TEST(BTStatus, Values) {
    EXPECT_EQ(static_cast<int>(BTStatus::Success), 0);
    EXPECT_EQ(static_cast<int>(BTStatus::Failure), 1);
    EXPECT_EQ(static_cast<int>(BTStatus::Running), 2);
}

TEST(BTNodeDef, DefaultConstruction) {
    BTNodeDef def;

    EXPECT_TRUE(def.Type.empty());
    EXPECT_TRUE(def.ChildIndices.empty());
}

TEST(BTNodeDef, WithValues) {
    BTNodeDef def;
    def.Type = "Sequence";
    def.ChildCount = 3;
    def.FirstChildIndex = 1;
    def.ChildIndices = {1, 2, 3};

    EXPECT_EQ(def.Type, "Sequence");
    EXPECT_EQ(def.ChildCount, 3);
    EXPECT_EQ(def.FirstChildIndex, 1);
    EXPECT_EQ(def.ChildIndices.size(), 3);
    EXPECT_EQ(def.ChildIndices[0], 1);
    EXPECT_EQ(def.ChildIndices[1], 2);
    EXPECT_EQ(def.ChildIndices[2], 3);
}

TEST(BehaviorTreeDef, DefaultConstruction) {
    BehaviorTreeDef def;

    EXPECT_TRUE(def.Nodes.empty());
    EXPECT_TRUE(def.Name.empty());
}

TEST(BehaviorTreeDef, WithNodes) {
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

    EXPECT_EQ(def.Nodes.size(), 3);
    EXPECT_EQ(def.GetRoot().Type, "Selector");
    EXPECT_EQ(def.GetRoot().ChildCount, 2);
    EXPECT_EQ(def.GetNode(1).Type, "Action");
}

TEST(BTExecutionState, ResetInitializesStack) {
    BTExecutionState state;
    state.Reset();
    EXPECT_EQ(state.StackDepth, 1);
    EXPECT_EQ(state.Current().NodeIndex, 0);
    EXPECT_EQ(state.Current().LastStatus, BTStatus::Running);
}

TEST(BTExecutionState, PushAddsToStack) {
    BTExecutionState state;
    state.Reset();
    state.Push(5);
    EXPECT_EQ(state.StackDepth, 2);
    EXPECT_EQ(state.Current().NodeIndex, 5);
}

TEST(BTExecutionState, PopRemovesFromStack) {
    BTExecutionState state;
    state.Reset();
    state.Push(5);
    state.Pop();
    EXPECT_EQ(state.StackDepth, 1);
    EXPECT_EQ(state.Current().NodeIndex, 0);
}
