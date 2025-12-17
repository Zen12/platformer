#pragma once

#include <memory>
#include <string>
#include "../node.hpp"
#include "../../../renderer/ui_manager.hpp"
#include "../../../scene/scene_manager.hpp"
#include "../../../renderer/ui/ui_page_action.hpp"
#include "../../../scene/load_scene_action.hpp"
#include "../../../renderer/ui/button_listener_action.hpp"

class ActionFactory {
private:
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<SceneManager> _sceneManager;

public:
    ActionFactory(std::shared_ptr<UIManager> uiManager, std::shared_ptr<SceneManager> sceneManager)
        : _uiManager(std::move(uiManager)), _sceneManager(std::move(sceneManager)) {}

    [[nodiscard]] UiPageAction CreateUiPageAction(const std::string& pageGuid) const {
        return {pageGuid, _uiManager};
    }

    [[nodiscard]] LoadSceneAction CreateLoadSceneAction(const std::string& sceneGuid) const {
        return {sceneGuid, _sceneManager};
    }

    [[nodiscard]] ButtonListenerAction CreateButtonListenerAction(const std::string& buttonId) const {
        return {buttonId, _uiManager};
    }
};