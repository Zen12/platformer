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
#include "../system/guid.hpp"

// Action serialization includes for dynamic_cast
#include "../renderer/ui/ui_page_action_serialization.hpp"
#include "../renderer/ui/button_listener_action_serialization.hpp"
#include "../renderer/ui/trigger_setter_button_listener_action_serialization.hpp"
#include "../scene/load_scene_action_serialization.hpp"
#include "node/action/set_system_trigger_action_serialization.hpp"
#include "node/action/log_action_serialization.hpp"
#include "node/action/animation_state_action_serialization.hpp"
#include "node/action/animation_state_transition_action_serialization.hpp"
#include "node/action/start_video_recording_action_serialization.hpp"
#include "node/action/stop_video_recording_action_serialization.hpp"
#include "node/action/fps_display_action_serialization.hpp"
#include "node/action/health_display_action_serialization.hpp"
#include "node/action/health_bar_action_serialization.hpp"
#include "node/action/health_check_action_serialization.hpp"
#include "../audio/play_sound_action_serialization.hpp"
#include "../audio/play_sound_repeated_action_serialization.hpp"
#include "../audio/mute_audio_action_serialization.hpp"

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

            for (const auto& actionSer : nodeSer->ActionData) {
                if (actionSer->Type == "load_ui_page") {
                    if (const auto* ser = dynamic_cast<UIPageActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateUiPageAction(ser->UiPageGuid));
                    }
                } else if (actionSer->Type == "load_scene") {
                    if (const auto* ser = dynamic_cast<LoadSceneActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateLoadSceneAction(ser->SceneGuid));
                    }
                } else if (actionSer->Type == "action_button_listener") {
                    if (const auto* ser = dynamic_cast<ButtonListenerActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateButtonListenerAction(ser->ButtonId));
                    }
                } else if (actionSer->Type == "action_trigger_setter_button_listener") {
                    if (const auto* ser = dynamic_cast<TriggerSetterButtonListenerActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateTriggerSetterButtonListenerAction(ser->ButtonId, ser->TriggerName));
                    }
                } else if (actionSer->Type == "set_system_trigger") {
                    if (const auto* ser = dynamic_cast<SetSystemTriggerActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateSetSystemTriggerAction(ser->TriggerType));
                    }
                } else if (actionSer->Type == "log") {
                    if (const auto* ser = dynamic_cast<LogActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateLogAction(ser->Message));
                    }
                } else if (actionSer->Type == "animation_state") {
                    if (const auto* ser = dynamic_cast<AnimationStateActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateAnimationStateAction(
                            Guid::FromString(ser->AnimationGuid),
                            ser->OnCompleteTrigger,
                            ser->Loop,
                            ser->AnimationSpeed,
                            ser->DisableVelocitySpeed,
                            ser->DisableMovement,
                            ser->DisableMovementDuration,
                            ser->UseDirectionalBlending,
                            Guid::FromString(ser->DirectionalWalkForwardGuid),
                            Guid::FromString(ser->DirectionalWalkBackGuid),
                            Guid::FromString(ser->DirectionalWalkLeftGuid),
                            Guid::FromString(ser->DirectionalWalkRightGuid)));
                    }
                } else if (actionSer->Type == "animation_state_transition") {
                    if (const auto* ser = dynamic_cast<AnimationStateTransitionActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateAnimationStateTransitionAction(
                            Guid::FromString(ser->FromAnimationGuid),
                            Guid::FromString(ser->ToAnimationGuid),
                            ser->TransitionTime,
                            ser->OnCompleteTrigger));
                    }
                } else if (actionSer->Type == "start_video_recording") {
                    if (const auto* ser = dynamic_cast<StartVideoRecordingActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateStartVideoRecordingAction(ser->OutputFile, ser->Fps));
                    }
                } else if (actionSer->Type == "stop_video_recording") {
                    actions.emplace_back(actionFactory.CreateStopVideoRecordingAction());
                } else if (actionSer->Type == "fps_display") {
                    if (const auto* ser = dynamic_cast<FpsDisplayActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateFpsDisplayAction(ser->ElementId));
                    }
                } else if (actionSer->Type == "health_display") {
                    if (const auto* ser = dynamic_cast<HealthDisplayActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateHealthDisplayAction(ser->ElementId));
                    }
                } else if (actionSer->Type == "health_bar") {
                    if (const auto* ser = dynamic_cast<HealthBarActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateHealthBarAction(ser->ElementId));
                    }
                } else if (actionSer->Type == "health_check") {
                    if (const auto* ser = dynamic_cast<HealthCheckActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateHealthCheckAction(ser->TriggerName));
                    }
                } else if (actionSer->Type == "play_sound") {
                    if (const auto* ser = dynamic_cast<PlaySoundActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreatePlaySoundAction(ser->AudioGuid, ser->Volume, ser->Loop));
                    }
                } else if (actionSer->Type == "play_sound_repeated") {
                    if (const auto* ser = dynamic_cast<PlaySoundRepeatedActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreatePlaySoundRepeatedAction(
                            ser->AudioGuid, ser->Volume, ser->DelaySeconds,
                            ser->Spatial, ser->MinDistance, ser->MaxDistance));
                    }
                } else if (actionSer->Type == "mute_audio") {
                    if (const auto* ser = dynamic_cast<MuteAudioActionSerialization*>(actionSer.get())) {
                        actions.emplace_back(actionFactory.CreateMuteAudioAction(ser->Mute, ser->OnlyDebug));
                    }
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
