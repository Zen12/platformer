#pragma once
#include <utility>
#include <vector>

#include "condition/condition.hpp"
#include "condition/core_types/trigger_check_condition.hpp"
#include "condition/core_types/trigger_check_condition_serialization.hpp"
#include "connection/connection.hpp"
#include "node/node.hpp"
#include "node/action/action_factory.hpp"
#include "fsm_asset.hpp"


class FsmController final {
private:
    std::unordered_map<std::string, StateNode> _stateNodes{};
    std::vector<Connection> _connections{};
    std::unordered_map<std::string, std::unique_ptr<Condition>> _conditions{};
    std::unordered_map<std::string, bool> _triggers{};
    // no support for subfsm??

    std::string _currentState;
    std::shared_ptr<SceneManager> _sceneManager;
    std::shared_ptr<UIManager> _uiManager;

    bool _wasEntered;

private:
    void ChangeState(const std::string &state) {
        _currentState = state;
        _wasEntered = false;
    }


public:
    explicit FsmController(const FsmAsset& fsmAsset, const std::shared_ptr<SceneManager> &sceneManager, const std::shared_ptr<UIManager> &uiManager)
        :   _currentState(fsmAsset.StartNode),
            _sceneManager (sceneManager),
            _uiManager(uiManager),
            _wasEntered(false)
    {
        // Create ActionFactory with dependencies
        ActionFactory actionFactory(uiManager, sceneManager, _triggers);

        // Convert StateNodeSerialization to StateNode
        for (const auto& nodeSer : fsmAsset.StateNodeSerialization) {
            std::vector<StateNode::AllActionVariants> actions;

            // Create actions from serialization data using ActionFactory
            for (const auto& actionData : nodeSer->ActionData) {
                if (actionData.Type == "load_ui_page") {
                    actions.emplace_back(actionFactory.CreateUiPageAction(actionData.Param));
                } else if (actionData.Type == "load_scene") {
                    actions.emplace_back(actionFactory.CreateLoadSceneAction(actionData.Param));
                } else if (actionData.Type == "action_button_listener") {
                    actions.emplace_back(actionFactory.CreateButtonListenerAction(actionData.Param));
                } else if (actionData.Type == "action_trigger_setter_button_listener") {
                    actions.emplace_back(actionFactory.CreateTriggerSetterButtonListenerAction(actionData.Param, actionData.Param2));
                }
            }

            StateNode node(nodeSer->Guid, std::move(actions));
            _stateNodes.emplace(nodeSer->Guid, std::move(node));
        }

        // Convert ConnectionSerialization to Connection
        for (const auto& connSer : fsmAsset.ConnectionSerialization) {
            Connection conn;
            conn.Nodes = connSer.Nodes;
            conn.ConditionGuids = connSer.ConditionGuids;
            conn.ConditionType = static_cast<::ConditionType>(connSer.ConditionType);
            _connections.push_back(conn);
        }

        // Convert ConditionSerialization to Condition with triggers injection
        for (const auto& condSer : fsmAsset.ConditionSerialization) {
            if (condSer->Type == "trigger_check") {
                if (const auto* triggerCheckSer = dynamic_cast<TriggerCheckConditionSerialization*>(condSer.get())) {
                    auto condition = std::make_unique<TriggerCheckCondition>(
                        triggerCheckSer->TriggerName,
                        _triggers
                    );
                    condition->Type = triggerCheckSer->Type;
                    condition->Guid = triggerCheckSer->Guid;
                    _conditions[triggerCheckSer->Guid] = std::move(condition);
                }
            }
        }

        std::cout << _conditions.size() << std::endl;
    }

    void Update() {
        // Reset triggers at the end of update, after condition checks
       // _triggers.clear();

        const auto currentNode = _stateNodes.at(_currentState);

        if (!_wasEntered) {
            currentNode.EnterAll();
            _wasEntered = true;
        }

        currentNode.UpdateAll();

        const auto &result = std::find_if(_connections.begin(), _connections.end(),
            [&](const Connection& conn) {
                return conn.Nodes[0] == _currentState;
            });

        // No connection found from current state
        if (result == _connections.end()) {
            return;
        }

        if (result->ConditionType == ConditionType::All) {
            for (const auto &conditionGuid: result->ConditionGuids) {
                auto condIt = _conditions.find(conditionGuid);
                if (condIt == _conditions.end()) {
                    // Condition not found, treat as failed
                    return;
                }
                const auto &condition = condIt->second;
                if (!condition->IsSuccess()) {
                    return; //fail
                }
            }
            ChangeState(result->Nodes[1]);
        }

        if (result->ConditionType == ConditionType::AtLeastOne) {
            bool anySuccess = false;
            for (const auto &conditionGuid: result->ConditionGuids) {
                auto condIt = _conditions.find(conditionGuid);
                if (condIt != _conditions.end() && condIt->second->IsSuccess()) {
                    anySuccess = true;
                    break; //success
                }
            }
            if (anySuccess) {
                ChangeState(result->Nodes[1]);
            }
        }
    }
};
