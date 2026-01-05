
#pragma once
#include "node_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "../../renderer/ui/ui_page_action_serialization.hpp"
#include "../../renderer/ui/ui_page_action_serialization_yaml.hpp"
#include "../../renderer/ui/button_listener_action.hpp"
#include "../../renderer/ui/button_listener_action_serialization.hpp"
#include "../../renderer/ui/button_listener_action_serialization_yaml.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action_serialization.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action_serialization_yaml.hpp"
#include "../../scene/load_scene_action.hpp"
#include "../../scene/load_scene_action_serialization.hpp"
#include "../../scene/load_scene_action_serialization_yaml.hpp"
#include "action/set_system_trigger_action.hpp"
#include "action/set_system_trigger_action_serialization.hpp"
#include "action/set_system_trigger_action_serialization_yaml.hpp"
#include "action/log_action.hpp"
#include "action/log_action_serialization.hpp"
#include "action/log_action_serialization_yaml.hpp"
#include "action/animation_state_action.hpp"
#include "action/animation_state_action_serialization.hpp"
#include "action/animation_state_action_serialization_yaml.hpp"
#include "action/animation_state_transition_action.hpp"
#include "action/animation_state_transition_action_serialization.hpp"
#include "action/animation_state_transition_action_serialization_yaml.hpp"
#include "action/start_video_recording_action.hpp"
#include "action/start_video_recording_action_serialization.hpp"
#include "action/start_video_recording_action_serialization_yaml.hpp"
#include "action/stop_video_recording_action.hpp"
#include "action/stop_video_recording_action_serialization.hpp"
#include "action/stop_video_recording_action_serialization_yaml.hpp"
#include "action/fps_display_action.hpp"
#include "action/fps_display_action_serialization.hpp"
#include "action/fps_display_action_serialization_yaml.hpp"
#include "action/action_serialization_data.hpp"

namespace YAML {
    template <>
    struct convert<StateNodeSerialization> {

        static ActionSerializationData DecodeAction(const YAML::Node& node) {
            const auto type = node["type"].as<std::string>();
            ActionSerializationData data;
            data.Type = type;

            if (type == "load_ui_page") {
                const auto serialization = node.as<UIPageActionSerialization>();
                data.Param = serialization.UiPageGuid;
            } else if (type == "load_scene") {
                const auto serialization = node.as<LoadSceneActionSerialization>();
                data.Param = serialization.SceneGuid;
            } else if (type == "action_button_listener") {
                const auto serialization = node.as<ButtonListenerActionSerialization>();
                data.Param = serialization.ButtonId;
            } else if (type == "action_trigger_setter_button_listener") {
                const auto serialization = node.as<TriggerSetterButtonListenerActionSerialization>();
                data.Param = serialization.ButtonId;
                data.Param2 = serialization.TriggerName;
            } else if (type == "set_system_trigger") {
                const auto serialization = node.as<SetSystemTriggerActionSerialization>();
                data.Param = serialization.TriggerType;
            } else if (type == "log") {
                const auto serialization = node.as<LogActionSerialization>();
                data.Param = serialization.Message;
            } else if (type == "animation_state") {
                const auto serialization = node.as<AnimationStateActionSerialization>();
                data.Param = serialization.AnimationGuid;
                data.Param2 = serialization.OnCompleteTrigger;
                data.Param3 = serialization.Loop ? "true" : "false";
                data.Param4 = std::to_string(serialization.AnimationSpeed);
                data.Param5 = serialization.DisableVelocitySpeed ? "true" : "false";
                data.Param6 = serialization.DisableMovement ? "true" : "false";
                data.Param7 = std::to_string(serialization.DisableMovementDuration);
            } else if (type == "animation_state_transition") {
                const auto serialization = node.as<AnimationStateTransitionActionSerialization>();
                data.Param = serialization.FromAnimationGuid;
                data.Param2 = serialization.ToAnimationGuid;
                data.Param3 = std::to_string(serialization.TransitionTime);
                data.Param4 = serialization.OnCompleteTrigger;
            } else if (type == "start_video_recording") {
                const auto serialization = node.as<StartVideoRecordingActionSerialization>();
                data.Param = serialization.OutputFile;
                data.Param2 = std::to_string(serialization.Fps);
            } else if (type == "stop_video_recording") {
                // No parameters needed for stop action
            } else if (type == "fps_display") {
                const auto serialization = node.as<FpsDisplayActionSerialization>();
                data.Param = serialization.ElementId;
            }

            return data;
        }

        static bool decode(const Node &node, StateNodeSerialization &rhs) {
            rhs.Guid = node["guid"].as<std::string>();
            for (const auto& comp : node["actions"]) {
                rhs.ActionData.push_back(DecodeAction(comp));
            }
            return true;
        }
    };
}
