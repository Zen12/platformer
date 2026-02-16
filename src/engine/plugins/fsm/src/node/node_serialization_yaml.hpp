#pragma once
#include "node_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include <functional>
#include <unordered_map>

// Action serialization includes
#include "ui/ui_page_action_serialization.hpp"
#include "ui/ui_page_action_serialization_yaml.hpp"
#include "ui/button_listener_action_serialization.hpp"
#include "ui/button_listener_action_serialization_yaml.hpp"
#include "ui/trigger_setter_button_listener_action_serialization.hpp"
#include "ui/trigger_setter_button_listener_action_serialization_yaml.hpp"
#include "load_scene_action_serialization.hpp"
#include "load_scene_action_serialization_yaml.hpp"
#include "action/set_system_trigger_action_serialization.hpp"
#include "action/set_system_trigger_action_serialization_yaml.hpp"
#include "action/log_action_serialization.hpp"
#include "action/log_action_serialization_yaml.hpp"
#include "action/animation_state_action_serialization.hpp"
#include "action/animation_state_action_serialization_yaml.hpp"
#include "action/animation_state_transition_action_serialization.hpp"
#include "action/animation_state_transition_action_serialization_yaml.hpp"
#include "action/start_video_recording_action_serialization.hpp"
#include "action/start_video_recording_action_serialization_yaml.hpp"
#include "action/stop_video_recording_action_serialization.hpp"
#include "action/stop_video_recording_action_serialization_yaml.hpp"
#include "action/fps_display_action_serialization.hpp"
#include "action/fps_display_action_serialization_yaml.hpp"
#include "action/health_display_action_serialization.hpp"
#include "action/health_display_action_serialization_yaml.hpp"
#include "action/health_bar_action_serialization.hpp"
#include "action/health_bar_action_serialization_yaml.hpp"
#include "action/health_check_action_serialization.hpp"
#include "action/health_check_action_serialization_yaml.hpp"
#include "play_sound_action_serialization.hpp"
#include "play_sound_action_serialization_yaml.hpp"
#include "play_sound_repeated_action_serialization.hpp"
#include "play_sound_repeated_action_serialization_yaml.hpp"
#include "mute_audio_action_serialization.hpp"
#include "mute_audio_action_serialization_yaml.hpp"

namespace YAML {
    template <>
    struct convert<StateNodeSerialization> {
        template<typename T>
        static std::unique_ptr<T> Parse(const YAML::Node &data) {
            static_assert(std::is_base_of_v<ActionSerialization, T>,
                      "T must derive from ActionSerialization");

            auto action = data.as<T>();
            return std::make_unique<T>(std::move(action));
        }

        static std::unique_ptr<ActionSerialization> DecodeAction(const YAML::Node& node) {
            using Factory = std::function<std::unique_ptr<ActionSerialization>(const YAML::Node&)>;
            static const std::unordered_map<std::string, Factory> factories = {
                { "load_ui_page", [](const YAML::Node& n){ return Parse<UIPageActionSerialization>(n); } },
                { "load_scene", [](const YAML::Node& n){ return Parse<LoadSceneActionSerialization>(n); } },
                { "action_button_listener", [](const YAML::Node& n){ return Parse<ButtonListenerActionSerialization>(n); } },
                { "action_trigger_setter_button_listener", [](const YAML::Node& n){ return Parse<TriggerSetterButtonListenerActionSerialization>(n); } },
                { "set_system_trigger", [](const YAML::Node& n){ return Parse<SetSystemTriggerActionSerialization>(n); } },
                { "log", [](const YAML::Node& n){ return Parse<LogActionSerialization>(n); } },
                { "animation_state", [](const YAML::Node& n){ return Parse<AnimationStateActionSerialization>(n); } },
                { "animation_state_transition", [](const YAML::Node& n){ return Parse<AnimationStateTransitionActionSerialization>(n); } },
                { "start_video_recording", [](const YAML::Node& n){ return Parse<StartVideoRecordingActionSerialization>(n); } },
                { "stop_video_recording", [](const YAML::Node& n){ return Parse<StopVideoRecordingActionSerialization>(n); } },
                { "fps_display", [](const YAML::Node& n){ return Parse<FpsDisplayActionSerialization>(n); } },
                { "health_display", [](const YAML::Node& n){ return Parse<HealthDisplayActionSerialization>(n); } },
                { "health_bar", [](const YAML::Node& n){ return Parse<HealthBarActionSerialization>(n); } },
                { "health_check", [](const YAML::Node& n){ return Parse<HealthCheckActionSerialization>(n); } },
                { "play_sound", [](const YAML::Node& n){ return Parse<PlaySoundActionSerialization>(n); } },
                { "play_sound_repeated", [](const YAML::Node& n){ return Parse<PlaySoundRepeatedActionSerialization>(n); } },
                { "mute_audio", [](const YAML::Node& n){ return Parse<MuteAudioActionSerialization>(n); } }
            };

            const auto type = node["type"].as<std::string>();
            if (const auto it = factories.find(type); it != factories.end()) {
                auto result = it->second(node);
                result->Type = type;
                return result;
            }
            return {};
        }

        static bool decode(const Node &node, StateNodeSerialization &rhs) {
            rhs.Guid = node["guid"].as<std::string>();
            if (node["actions"]) {
                for (const auto& action : node["actions"]) {
                    if (auto decoded = DecodeAction(action)) {
                        rhs.ActionData.push_back(std::move(decoded));
                    }
                }
            }
            return true;
        }
    };
}
