#pragma once
#include "bt_node.hpp"
#include "all_bt_node_types.hpp"
#include <memory>
#include <unordered_map>
#include <string>

class BTNodeRegistry {
public:
    BTNodeRegistry() {
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

    IBTNode* GetNode(const std::string& type) const {
        auto it = _nodes.find(type);
        if (it != _nodes.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    template<typename T>
    void Register(const std::string& type) {
        _nodes[type] = std::make_unique<T>();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IBTNode>> _nodes;
};
