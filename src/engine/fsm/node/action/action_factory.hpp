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

class ActionFactory {
private:
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, bool>& _triggers;
    std::unordered_map<SystemTriggers, bool>& _systemTriggers;

public:
    ActionFactory(std::shared_ptr<UIManager> uiManager, std::shared_ptr<SceneManager> sceneManager, std::unordered_map<std::string, bool>& triggers, std::unordered_map<SystemTriggers, bool>& systemTriggers)
        : _uiManager(std::move(uiManager)), _sceneManager(std::move(sceneManager)), _triggers(triggers), _systemTriggers(systemTriggers) {}

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
};