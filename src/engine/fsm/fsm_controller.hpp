#pragma once
#include <utility>
#include <vector>

#include "condition/condition.hpp"
#include "condition/core_types/trigger_check_condition.hpp"
#include "condition/core_types/trigger_check_condition_serialization.hpp"
#include "connection/connection.hpp"
#include "node/node.hpp"
#include "node/action/action_factory.hpp"
#include "node/action/set_system_trigger_action.hpp"
#include "node/action/log_action.hpp"
#include "fsm_asset.hpp"
#include "system_triggers.hpp"

// Forward declaration
class FsmAnimationComponent;

class FsmController final {
private:
    std::unordered_map<std::string, StateNode> _stateNodes{};
    std::vector<Connection> _connections{};
    std::unordered_map<std::string, std::unique_ptr<Condition>> _conditions{};
    std::unordered_map<std::string, bool> _triggers{};
    std::unordered_map<SystemTriggers, bool> _systemTrigger{};

    std::string _currentState;
    std::shared_ptr<SceneManager> _sceneManager;
    std::shared_ptr<UIManager> _uiManager;

    bool _wasEntered;

private:
    void ChangeState(const std::string &state) {
        auto currentNode = _stateNodes.at(_currentState);
        currentNode.ExitAll();

        _currentState = state;
        _wasEntered = false;
        _triggers.clear();
    }


public:
    explicit FsmController(const FsmAsset& fsmAsset, const std::shared_ptr<SceneManager> &sceneManager, const std::shared_ptr<UIManager> &uiManager, const std::weak_ptr<FsmAnimationComponent> &animationComponent = std::weak_ptr<FsmAnimationComponent>())
        :   _currentState(fsmAsset.StartNode),
            _sceneManager (sceneManager),
            _uiManager(uiManager),
            _wasEntered(false)
    {
        // Create ActionFactory with dependencies
        ActionFactory actionFactory(uiManager, sceneManager, _triggers, _systemTrigger, animationComponent);

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
                } else if (actionData.Type == "set_system_trigger") {
                    actions.emplace_back(actionFactory.CreateSetSystemTriggerAction(actionData.Param));
                } else if (actionData.Type == "log") {
                    actions.emplace_back(actionFactory.CreateLogAction(actionData.Param));
                } else if (actionData.Type == "animation_state") {
                    actions.emplace_back(actionFactory.CreateAnimationStateAction(actionData.Param, actionData.Param2));
                } else if (actionData.Type == "animation_state_transition") {
                    actions.emplace_back(actionFactory.CreateAnimationStateTransitionAction(actionData.Param, actionData.Param2, std::stof(actionData.Param3), actionData.Param4));
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
    }

    [[nodiscard]] bool IsSystemTriggered(const SystemTriggers &triggerName) const {
        if (_systemTrigger.find(triggerName) == _systemTrigger.end())
            return false;

        return _systemTrigger.at(triggerName);
    }

    void SetTrigger(const std::string& triggerName) {
        _triggers[triggerName] = true;
    }

    void Update() {
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
            return;
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
                return;
            }
        }
    }
};
