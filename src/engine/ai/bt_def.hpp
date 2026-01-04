#pragma once
#include "bt_node_type.hpp"
#include <vector>
#include <string>
#include <cstdint>

struct BTNodeDef {
    BTNodeType Type;
    uint8_t ChildCount;
    uint16_t FirstChildIndex;
    float Param1;           // Distance threshold, wait time, etc.
    float Param2;           // Secondary parameter if needed
    std::string StringParam; // Tag name, etc.
};

struct BehaviorTreeDef {
    std::string Name;
    std::vector<BTNodeDef> Nodes;

    [[nodiscard]] const BTNodeDef& GetRoot() const { return Nodes[0]; }
    [[nodiscard]] const BTNodeDef& GetNode(uint16_t index) const { return Nodes[index]; }
    [[nodiscard]] uint16_t GetChildIndex(const BTNodeDef& parent, uint8_t childOffset) const {
        return parent.FirstChildIndex + childOffset;
    }
};

struct BTNodeState {
    uint16_t NodeIndex;
    uint8_t ChildProgress;  // For sequence/selector: which child we're on
    float Timer;            // For Wait node
    BTStatus LastStatus;
};

// Stack-based execution state (avoids recursion, cache-friendly)
struct BTExecutionState {
    static constexpr size_t MaxDepth = 16;
    BTNodeState Stack[MaxDepth];
    uint8_t StackDepth;

    void Reset() {
        StackDepth = 1;
        Stack[0] = {0, 0, 0.0f, BTStatus::Running};
    }

    void Push(uint16_t nodeIndex) {
        if (StackDepth < MaxDepth) {
            Stack[StackDepth++] = {nodeIndex, 0, 0.0f, BTStatus::Running};
        }
    }

    void Pop() {
        if (StackDepth > 0) StackDepth--;
    }

    [[nodiscard]] BTNodeState& Current() { return Stack[StackDepth - 1]; }
    [[nodiscard]] const BTNodeState& Current() const { return Stack[StackDepth - 1]; }
    [[nodiscard]] bool IsEmpty() const { return StackDepth == 0; }
};
