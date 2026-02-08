#pragma once
#include <any>
#include <variant>
#include <vector>
#include <typeinfo>

#include "../../renderer/ui/ui_page_action.hpp"
#include "../../renderer/ui/button_listener_action.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action.hpp"
#include "../../scene/load_scene_action.hpp"
#include "action/set_system_trigger_action.hpp"
#include "action/log_action.hpp"
#include "action/animation_state_action.hpp"
#include "action/animation_state_transition_action.hpp"
#include "action/start_video_recording_action.hpp"
#include "action/stop_video_recording_action.hpp"
#include "action/fps_display_action.hpp"
#include "action/health_display_action.hpp"
#include "action/health_bar_action.hpp"
#include "action/health_check_action.hpp"
#include "../../audio/play_sound_action.hpp"
#include "../../audio/play_sound_repeated_action.hpp"
#include "../../audio/mute_audio_action.hpp"

class StateNode final {
public:
    using AllActionVariants = std::variant<UiPageAction, LoadSceneAction, ButtonListenerAction, TriggerSetterButtonListenerAction, SetSystemTriggerAction, LogAction, AnimationStateAction, AnimationStateTransitionAction, StartVideoRecordingAction, StopVideoRecordingAction, FpsDisplayAction, HealthDisplayAction, HealthBarAction, HealthCheckAction, PlaySoundAction, PlaySoundRepeatedAction, MuteAudioAction>;
private:
    mutable std::vector<AllActionVariants> _actions{};
public:
    const std::string Guid;

    StateNode(std::string guid, std::vector<AllActionVariants> actions)
        : _actions(std::move(actions)), Guid(std::move(guid)) {}

    void EnterAll() const {
        for (auto& action : _actions) {
            std::visit([](auto& a) {
                a.OnEnter();
            }, action);
        }
    }

    void UpdateAll() const {
        for (auto& action : _actions) {
            std::visit([](auto& a) {
                a.OnUpdate();
            }, action);
        }
    }

    void ExitAll() const {
        for (auto& action : _actions) {
            std::visit([](auto& a) {
                a.OnExit();
            }, action);
        }
    }
};
