#pragma once
#include "bt_node.hpp"
#include "all_bt_node_types.hpp"
#include <memory>
#include <unordered_map>
#include <string>

class BTNodeRegistry {
public:
    static BTNodeRegistry& Instance() {
        static BTNodeRegistry instance;
        return instance;
    }

    IBTNode* GetNode(const std::string& type) {
        auto it = _nodes.find(type);
        if (it != _nodes.end()) {
            return it->second.get();
        }
        return nullptr;
    }

private:
    BTNodeRegistry() {
        Register<SequenceNode>("sequence");
        Register<SelectorNode>("selector");
        Register<WaitNode>("wait");
        Register<HasTargetNode>("has_target");
        Register<ClearTargetNode>("clear_target");

        Register<FindTargetByTagNode>("find_target_by_tag");
        Register<HasTargetInRangeNode>("has_target_in_range");
        Register<CheckDistanceNode>("check_distance");

        Register<IsMovingNode>("is_moving");
        Register<MoveToTargetNode>("move_to_target");
        Register<RandomWanderNode>("random_wander");
        Register<IdleNode>("idle");

        Register<AttackNode>("attack");
    }

    template<typename T>
    void Register(const std::string& type) {
        _nodes[type] = std::make_unique<T>();
    }

    std::unordered_map<std::string, std::unique_ptr<IBTNode>> _nodes;
};
