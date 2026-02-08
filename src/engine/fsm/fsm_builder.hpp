#pragma once

#include <memory>
#include <string>

#include "fsm_data.hpp"
#include "fsm_asset.hpp"
#include "trigger_board.hpp"
#include "node/action/action_factory.hpp"
#include "condition/core_types/trigger_check_condition.hpp"
#include "condition/core_types/trigger_check_condition_serialization.hpp"
#include "condition/core_types/always_true_condition.hpp"
#include "condition/core_types/always_true_condition_serialization.hpp"

class FsmAnimationComponent;
class VideoRecorder;
class AudioManager;
class UIManager;
class SceneManager;

class FsmBuilder final {
public:
    static FsmData Build(
        const FsmAsset& fsmAsset,
        const std::shared_ptr<TriggerBoard>& triggerBoard,
        const std::shared_ptr<UIManager>& uiManager,
        const std::shared_ptr<SceneManager>& sceneManager,
        const std::shared_ptr<VideoRecorder>& videoRecorder = nullptr,
        const std::shared_ptr<AudioManager>& audioManager = nullptr,
        const std::weak_ptr<FsmAnimationComponent>& animationComponent = std::weak_ptr<FsmAnimationComponent>()
    ) {
        ActionFactory actionFactory(uiManager, sceneManager, triggerBoard,
                                    videoRecorder, audioManager, animationComponent);

        std::unordered_map<std::string, StateNode> stateNodes;
        std::vector<Connection> connections;
        std::unordered_map<std::string, AllConditionVariants> conditions;

        // Build StateNodes from serialization
        for (const auto& nodeSer : fsmAsset.StateNodeSerialization) {
            std::vector<StateNode::AllActionVariants> actions;

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
                    bool loop = actionData.Param3 != "false";
                    float animSpeed = actionData.Param4.empty() ? -1.0f : std::stof(actionData.Param4);
                    bool disableVelSpeed = actionData.Param5 == "true";
                    bool disableMovement = actionData.Param6 == "true";
                    float disableMovementDuration = actionData.Param7.empty() ? -1.0f : std::stof(actionData.Param7);
                    bool useDirectionalBlending = actionData.Param8 == "true";
                    actions.emplace_back(actionFactory.CreateAnimationStateAction(
                        actionData.Param, actionData.Param2, loop, animSpeed, disableVelSpeed, disableMovement, disableMovementDuration,
                        useDirectionalBlending, actionData.Param9, actionData.Param10, actionData.Param11, actionData.Param12));
                } else if (actionData.Type == "animation_state_transition") {
                    actions.emplace_back(actionFactory.CreateAnimationStateTransitionAction(actionData.Param, actionData.Param2, std::stof(actionData.Param3), actionData.Param4));
                } else if (actionData.Type == "start_video_recording") {
                    int fps = !actionData.Param2.empty() ? std::stoi(actionData.Param2) : 60;
                    actions.emplace_back(actionFactory.CreateStartVideoRecordingAction(actionData.Param, fps));
                } else if (actionData.Type == "stop_video_recording") {
                    actions.emplace_back(actionFactory.CreateStopVideoRecordingAction());
                } else if (actionData.Type == "fps_display") {
                    actions.emplace_back(actionFactory.CreateFpsDisplayAction(actionData.Param));
                } else if (actionData.Type == "health_display") {
                    actions.emplace_back(actionFactory.CreateHealthDisplayAction(actionData.Param));
                } else if (actionData.Type == "health_bar") {
                    actions.emplace_back(actionFactory.CreateHealthBarAction(actionData.Param));
                } else if (actionData.Type == "health_check") {
                    actions.emplace_back(actionFactory.CreateHealthCheckAction(actionData.Param));
                } else if (actionData.Type == "play_sound") {
                    float volume = actionData.Param2.empty() ? 1.0f : std::stof(actionData.Param2);
                    bool loop = actionData.Param3 == "true";
                    actions.emplace_back(actionFactory.CreatePlaySoundAction(actionData.Param, volume, loop));
                } else if (actionData.Type == "play_sound_repeated") {
                    float volume = actionData.Param2.empty() ? 1.0f : std::stof(actionData.Param2);
                    float delaySeconds = actionData.Param3.empty() ? 0.5f : std::stof(actionData.Param3);
                    bool spatial = actionData.Param4.empty() || actionData.Param4 == "true";
                    float minDistance = actionData.Param5.empty() ? 1.0f : std::stof(actionData.Param5);
                    float maxDistance = actionData.Param6.empty() ? 50.0f : std::stof(actionData.Param6);
                    actions.emplace_back(actionFactory.CreatePlaySoundRepeatedAction(actionData.Param, volume, delaySeconds, spatial, minDistance, maxDistance));
                } else if (actionData.Type == "mute_audio") {
                    bool mute = actionData.Param.empty() || actionData.Param == "true";
                    bool onlyDebug = actionData.Param2 == "true";
                    actions.emplace_back(actionFactory.CreateMuteAudioAction(mute, onlyDebug));
                }
            }

            StateNode node(nodeSer->Guid, std::move(actions));
            stateNodes.emplace(nodeSer->Guid, std::move(node));
        }

        // Build Connections from serialization
        for (const auto& connSer : fsmAsset.ConnectionSerialization) {
            Connection conn;
            conn.Nodes = connSer.Nodes;
            conn.ConditionGuids = connSer.ConditionGuids;
            conn.ConditionType = static_cast<::ConditionType>(connSer.ConditionType);
            connections.push_back(conn);
        }

        // Build Conditions from serialization
        for (const auto& condSer : fsmAsset.ConditionSerialization) {
            if (condSer->Type == "trigger_check") {
                if (const auto* triggerCheckSer = dynamic_cast<TriggerCheckConditionSerialization*>(condSer.get())) {
                    TriggerCheckCondition condition(triggerCheckSer->TriggerName, triggerBoard);
                    condition.Type = triggerCheckSer->Type;
                    condition.Guid = triggerCheckSer->Guid;
                    conditions[triggerCheckSer->Guid] = std::move(condition);
                }
            } else if (condSer->Type == "always_true") {
                AlwaysTrueCondition condition;
                condition.Type = condSer->Type;
                condition.Guid = condSer->Guid;
                conditions[condSer->Guid] = std::move(condition);
            }
        }

        return FsmData{
            std::move(stateNodes),
            std::move(connections),
            std::move(conditions),
            fsmAsset.StartNode
        };
    }
};
