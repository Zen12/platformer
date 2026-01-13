#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "../node.hpp"
#include "../../../renderer/ui_manager.hpp"
#include "../../../scene/scene_manager.hpp"
#include "../../../renderer/ui/ui_page_action.hpp"
#include "../../../scene/load_scene_action.hpp"
#include "../../../renderer/ui/button_listener_action.hpp"
#include "../../../renderer/ui/trigger_setter_button_listener_action.hpp"
#include "set_system_trigger_action.hpp"
#include "log_action.hpp"
#include "animation_state_action.hpp"
#include "animation_state_transition_action.hpp"
#include "start_video_recording_action.hpp"
#include "stop_video_recording_action.hpp"
#include "fps_display_action.hpp"
#include "health_display_action.hpp"
#include "health_bar_action.hpp"
#include "health_check_action.hpp"
#include "../../../audio/play_sound_action.hpp"
#include "../../../audio/play_sound_repeated_action.hpp"
#include "../../../audio/mute_audio_action.hpp"

// Forward declaration
class VideoRecorder;

class ActionFactory {
private:
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, bool>& _triggers;
    std::unordered_map<SystemTriggers, bool>& _systemTriggers;
    std::shared_ptr<VideoRecorder> _videoRecorder;  // Optional, for video recording
    std::shared_ptr<AudioManager> _audioManager;  // Optional, for audio
    std::weak_ptr<FsmAnimationComponent> _animationComponent;  // Optional, for animation FSMs

public:
    ActionFactory(std::shared_ptr<UIManager> uiManager,
                  std::shared_ptr<SceneManager> sceneManager,
                  std::unordered_map<std::string, bool>& triggers,
                  std::unordered_map<SystemTriggers, bool>& systemTriggers,
                  std::shared_ptr<VideoRecorder> videoRecorder = nullptr,
                  std::shared_ptr<AudioManager> audioManager = nullptr,
                  std::weak_ptr<FsmAnimationComponent> animationComponent = std::weak_ptr<FsmAnimationComponent>())
        : _uiManager(std::move(uiManager)),
          _sceneManager(std::move(sceneManager)),
          _triggers(triggers),
          _systemTriggers(systemTriggers),
          _videoRecorder(std::move(videoRecorder)),
          _audioManager(std::move(audioManager)),
          _animationComponent(std::move(animationComponent)) {}

    [[nodiscard]] UiPageAction CreateUiPageAction(const std::string& pageGuid) const {
        return {pageGuid, _uiManager};
    }

    [[nodiscard]] LoadSceneAction CreateLoadSceneAction(const std::string& sceneGuid) const {
        return {sceneGuid, _sceneManager};
    }

    [[nodiscard]] ButtonListenerAction CreateButtonListenerAction(const std::string& buttonId) const {
        return {buttonId, _uiManager};
    }

    [[nodiscard]] TriggerSetterButtonListenerAction CreateTriggerSetterButtonListenerAction(const std::string& buttonId, const std::string& triggerName) const {
        return {buttonId, triggerName, _uiManager, _triggers};
    }

    [[nodiscard]] SetSystemTriggerAction CreateSetSystemTriggerAction(const std::string& triggerTypeStr) const {
        SystemTriggers triggerType;
        if (triggerTypeStr == "Exit") {
            triggerType = SystemTriggers::Exit;
        } else if (triggerTypeStr == "Reload") {
            triggerType = SystemTriggers::Reload;
        } else {
            triggerType = SystemTriggers::Exit;
        }
        return {triggerType, _systemTriggers};
    }

    [[nodiscard]] LogAction CreateLogAction(const std::string& message) const {
        return LogAction(message);
    }

    [[nodiscard]] AnimationStateAction CreateAnimationStateAction(const std::string& animationGuid, const std::string& onCompleteTrigger = "", bool loop = true, float animationSpeed = -1.0f, bool disableVelocitySpeed = false, bool disableMovement = false, float disableMovementDuration = -1.0f) const {
        return {animationGuid, onCompleteTrigger, loop, animationSpeed, disableVelocitySpeed, disableMovement, disableMovementDuration, _animationComponent};
    }

    [[nodiscard]] AnimationStateTransitionAction CreateAnimationStateTransitionAction(const std::string& fromAnimationGuid, const std::string& toAnimationGuid, float transitionTime, const std::string& onCompleteTrigger = "") const {
        return {fromAnimationGuid, toAnimationGuid, transitionTime, onCompleteTrigger, _animationComponent};
    }

    [[nodiscard]] StartVideoRecordingAction CreateStartVideoRecordingAction(const std::string& outputPath, int fps) const {
        return {outputPath, fps, _videoRecorder};
    }

    [[nodiscard]] StopVideoRecordingAction CreateStopVideoRecordingAction() const {
        return StopVideoRecordingAction(_videoRecorder);
    }

    [[nodiscard]] FpsDisplayAction CreateFpsDisplayAction(const std::string& elementId) const {
        return {elementId, _uiManager, _sceneManager};
    }

    [[nodiscard]] HealthDisplayAction CreateHealthDisplayAction(const std::string& elementId) const {
        return {elementId, _uiManager, _sceneManager};
    }

    [[nodiscard]] HealthBarAction CreateHealthBarAction(const std::string& elementId) const {
        return {elementId, _uiManager, _sceneManager};
    }

    [[nodiscard]] HealthCheckAction CreateHealthCheckAction(const std::string& triggerName) const {
        return {triggerName, _sceneManager, _triggers};
    }

    [[nodiscard]] PlaySoundAction CreatePlaySoundAction(const std::string& audioGuid, float volume, bool loop) const {
        return {audioGuid, volume, loop, _audioManager};
    }

    [[nodiscard]] PlaySoundRepeatedAction CreatePlaySoundRepeatedAction(const std::string& audioGuid, float volume, float delaySeconds, bool spatial = true, float minDistance = 1.0f, float maxDistance = 50.0f) const {
        return {audioGuid, volume, delaySeconds, spatial, minDistance, maxDistance, _audioManager, _animationComponent};
    }

    [[nodiscard]] MuteAudioAction CreateMuteAudioAction(bool mute, bool onlyDebug) const {
        return {mute, onlyDebug, _audioManager};
    }
};