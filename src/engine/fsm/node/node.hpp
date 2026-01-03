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

class StateNode final {
public:
    using AllActionVariants = std::variant<UiPageAction, LoadSceneAction, ButtonListenerAction, TriggerSetterButtonListenerAction, SetSystemTriggerAction, LogAction, AnimationStateAction, AnimationStateTransitionAction, StartVideoRecordingAction, StopVideoRecordingAction, FpsDisplayAction>;
private:
    std::vector<AllActionVariants> _states{};
public:
    const std::string Guid;

    StateNode(std::string guid, std::vector<AllActionVariants> states)
        : _states(std::move(states)), Guid(std::move(guid)) {}

    void EnterAll() const {
        for (auto& stateVar : _states) {
            std::visit([](auto& state) {
                state.OnEnter();
            }, stateVar);
        }
    }

    void UpdateAll() const {
        for (auto &stateVar: _states) {
            std::visit([](auto& state) {
                state.OnUpdate();
            }, stateVar);
        }
    }

    void ExitAll() const {
        for (auto& stateVar : _states) {
            std::visit([](auto& state) {
                state.OnExit();
            }, stateVar);
        }
    }

};