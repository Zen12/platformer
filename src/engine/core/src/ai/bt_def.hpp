#pragma once
#include "bt_status.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstdint>

struct BTNodeDef {
    std::string Type;
    uint8_t ChildCount;
    uint16_t FirstChildIndex;
    std::vector<uint16_t> ChildIndices;
    float Param1;
    float Param2;
    std::string StringParam;
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
    uint8_t ChildProgress;
    float Timer;
    BTStatus LastStatus;
    glm::vec3 CustomTarget{0};
    bool HasCustomTarget = false;
};

struct BTExecutionState {
    static constexpr size_t MaxDepth = 16;
    BTNodeState Stack[MaxDepth];
    uint8_t StackDepth;

    void Reset() {
        StackDepth = 1;
        Stack[0] = {0, 0, 0.0f, BTStatus::Running, glm::vec3{0}, false};
    }

    void Push(uint16_t nodeIndex) {
        if (StackDepth < MaxDepth) {
            Stack[StackDepth++] = {nodeIndex, 0, 0.0f, BTStatus::Running, glm::vec3{0}, false};
        }
    }

    void Pop() {
        if (StackDepth > 0) StackDepth--;
    }

    [[nodiscard]] BTNodeState& Current() { return Stack[StackDepth - 1]; }
    [[nodiscard]] const BTNodeState& Current() const { return Stack[StackDepth - 1]; }
    [[nodiscard]] bool IsEmpty() const { return StackDepth == 0; }
};
