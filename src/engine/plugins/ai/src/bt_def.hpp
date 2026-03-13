#pragma once
#include "bt_status.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

enum class BTCompositeType : uint8_t {
    NONE = 0,
    SEQUENCE,
    SELECTOR,
    REPEATER,
};

inline BTCompositeType BTCompositeFromString(const std::string& str) {
    static const std::unordered_map<std::string, BTCompositeType> map = {
        {"sequence", BTCompositeType::SEQUENCE},
        {"selector", BTCompositeType::SELECTOR},
        {"repeater", BTCompositeType::REPEATER},
    };
    auto it = map.find(str);
    return it != map.end() ? it->second : BTCompositeType::NONE;
}

struct BTNodeDef {
    BTCompositeType Composite;
    std::string Type;
    uint8_t ChildCount;
    uint16_t FirstChildIndex;
    std::vector<uint16_t> ChildIndices;
    std::unordered_map<std::string, float> Floats;
    std::unordered_map<std::string, std::string> Strings;

    [[nodiscard]] float GetFloat(const std::string& key, float fallback = 0.0f) const {
        auto it = Floats.find(key);
        return it != Floats.end() ? it->second : fallback;
    }

    [[nodiscard]] const std::string& GetString(const std::string& key) const {
        static const std::string empty;
        auto it = Strings.find(key);
        return it != Strings.end() ? it->second : empty;
    }
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
    BTStatus LastStatus;
    std::unordered_map<std::string, float> Floats;
    std::unordered_map<std::string, glm::vec3> Vectors;
    std::unordered_map<std::string, bool> Bools;

    float& Float(const std::string& key) { return Floats[key]; }
    [[nodiscard]] float GetFloat(const std::string& key, float fallback = 0.0f) const {
        auto it = Floats.find(key);
        return it != Floats.end() ? it->second : fallback;
    }

    glm::vec3& Vec3(const std::string& key) { return Vectors[key]; }
    [[nodiscard]] glm::vec3 GetVec3(const std::string& key) const {
        auto it = Vectors.find(key);
        return it != Vectors.end() ? it->second : glm::vec3{0};
    }

    bool& Bool(const std::string& key) { return Bools[key]; }
    [[nodiscard]] bool GetBool(const std::string& key) const {
        auto it = Bools.find(key);
        return it != Bools.end() && it->second;
    }
};

struct BTExecutionState {
    static constexpr size_t MaxDepth = 16;
    BTNodeState Stack[MaxDepth];
    uint8_t StackDepth;

    void Reset() {
        StackDepth = 1;
        Stack[0] = {0, 0, BTStatus::Running, {}, {}, {}};
    }

    void Push(uint16_t nodeIndex) {
        if (StackDepth < MaxDepth) {
            Stack[StackDepth++] = {nodeIndex, 0, BTStatus::Running, {}, {}, {}};
        }
    }

    void Pop() {
        if (StackDepth > 0) StackDepth--;
    }

    [[nodiscard]] BTNodeState& Current() { return Stack[StackDepth - 1]; }
    [[nodiscard]] const BTNodeState& Current() const { return Stack[StackDepth - 1]; }
    [[nodiscard]] bool IsEmpty() const { return StackDepth == 0; }
};
