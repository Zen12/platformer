#pragma once
#include <utility>
#include <vector>
#include <iostream>

#define DEBUG_FSM_CONTROLLER 0

#if DEBUG_FSM_CONTROLLER
#define FSM_LOG if(1) std::cout
#else
#define FSM_LOG if(0) std::cout
#endif

#include "condition/condition.hpp"
#include "condition/core_types/trigger_check_condition.hpp"
#include "condition/core_types/trigger_check_condition_serialization.hpp"
#include "condition/core_types/always_true_condition.hpp"
#include "condition/core_types/always_true_condition_serialization.hpp"
#include "connection/connection.hpp"
#include "node/node.hpp"
#include "node/action/action_factory.hpp"
#include "node/action/set_system_trigger_action.hpp"
#include "node/action/log_action.hpp"
#include "node/action/fps_display_action.hpp"
#include "fsm_asset.hpp"
#include "system_triggers.hpp"

// Forward declarations
class FsmAnimationComponent;
class VideoRecorder;

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
    std::shared_ptr<VideoRecorder> _videoRecorder;

    bool _wasEntered;

private:
    void ChangeState(const std::string &state) {
        FSM_LOG << "[FSM] State transition: " << _currentState << " -> " << state << std::endl;

        auto currentNode = _stateNodes.at(_currentState);
        currentNode.ExitAll();

        _currentState = state;
        _wasEntered = false;
        _triggers.clear();
    }


public:
    explicit FsmController(const FsmAsset& fsmAsset, const std::shared_ptr<SceneManager> &sceneManager, const std::shared_ptr<UIManager> &uiManager, const std::shared_ptr<VideoRecorder> &videoRecorder = nullptr, const std::weak_ptr<FsmAnimationComponent> &animationComponent = std::weak_ptr<FsmAnimationComponent>())
        :   _currentState(fsmAsset.StartNode),
            _sceneManager (sceneManager),
            _uiManager(uiManager),
            _videoRecorder(videoRecorder),
            _wasEntered(false)
    {
        // Create ActionFactory with dependencies
        ActionFactory actionFactory(uiManager, sceneManager, _triggers, _systemTrigger, videoRecorder, animationComponent);

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
                    bool loop = actionData.Param3 != "false";  // Default to true if not specified
                    float animSpeed = actionData.Param4.empty() ? -1.0f : std::stof(actionData.Param4);
                    bool disableVelSpeed = actionData.Param5 == "true";
                    bool disableMovement = actionData.Param6 == "true";
                    float disableMovementDuration = actionData.Param7.empty() ? -1.0f : std::stof(actionData.Param7);
                    actions.emplace_back(actionFactory.CreateAnimationStateAction(actionData.Param, actionData.Param2, loop, animSpeed, disableVelSpeed, disableMovement, disableMovementDuration));
                } else if (actionData.Type == "animation_state_transition") {
                    actions.emplace_back(actionFactory.CreateAnimationStateTransitionAction(actionData.Param, actionData.Param2, std::stof(actionData.Param3), actionData.Param4));
                } else if (actionData.Type == "start_video_recording") {
                    // Param: output_file, Param2: fps
                    int fps = !actionData.Param2.empty() ? std::stoi(actionData.Param2) : 60;
                    actions.emplace_back(actionFactory.CreateStartVideoRecordingAction(actionData.Param, fps));
                } else if (actionData.Type == "stop_video_recording") {
                    actions.emplace_back(actionFactory.CreateStopVideoRecordingAction());
                } else if (actionData.Type == "fps_display") {
                    actions.emplace_back(actionFactory.CreateFpsDisplayAction(actionData.Param));
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
            } else if (condSer->Type == "always_true") {
                auto condition = std::make_unique<AlwaysTrueCondition>();
                condition->Type = condSer->Type;
                condition->Guid = condSer->Guid;
                _conditions[condSer->Guid] = std::move(condition);
            }
        }
    }

    [[nodiscard]] bool IsSystemTriggered(const SystemTriggers &triggerName) const {
        if (_systemTrigger.find(triggerName) == _systemTrigger.end())
            return false;

        return _systemTrigger.at(triggerName);
    }

    void SetTrigger(const std::string& triggerName) {
        FSM_LOG << "[FSM] Trigger set: " << triggerName << " (current state: " << _currentState << ")" << std::endl;
        _triggers[triggerName] = true;
    }

    void Update() {
        const auto currentNode = _stateNodes.at(_currentState);

        if (!_wasEntered) {
            FSM_LOG << "[FSM] Entering state: " << _currentState << std::endl;
            currentNode.EnterAll();
            _wasEntered = true;
        }

        currentNode.UpdateAll();

        // Check ALL connections from current state (not just the first one)
        for (const auto& conn : _connections) {
            if (conn.Nodes[0] != _currentState) {
                continue;
            }

            bool conditionsPassed = false;

            if (conn.ConditionType == ConditionType::All) {
                conditionsPassed = true;
                for (const auto &conditionGuid: conn.ConditionGuids) {
                    auto condIt = _conditions.find(conditionGuid);
                    if (condIt == _conditions.end() || !condIt->second->IsSuccess()) {
                        conditionsPassed = false;
                        break;
                    }
                }
            } else if (conn.ConditionType == ConditionType::AtLeastOne) {
                for (const auto &conditionGuid: conn.ConditionGuids) {
                    auto condIt = _conditions.find(conditionGuid);
                    if (condIt != _conditions.end() && condIt->second->IsSuccess()) {
                        conditionsPassed = true;
                        break;
                    }
                }
            }

            if (conditionsPassed) {
                ChangeState(conn.Nodes[1]);
                return;
            }
        }
    }
};
