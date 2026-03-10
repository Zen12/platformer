#pragma once

#include "action.hpp"
#include "ui_manager.hpp"
#include "scene_manager.hpp"
#include "time/time_component.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

struct FpsDisplayAction final : public Action {

private:
    std::string _elementId;
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<SceneManager> _sceneManager;

public:
    FpsDisplayAction(std::string elementId, std::shared_ptr<UIManager> uiManager, std::shared_ptr<SceneManager> sceneManager)
        : _elementId(std::move(elementId)), _uiManager(std::move(uiManager)), _sceneManager(std::move(sceneManager)) {}

    void OnEnter() const override {
        // No initialization needed
    }

    void OnUpdate() const override {
        if (_uiManager && _sceneManager && !_elementId.empty()) {
            // Get delta time from scene's ECS registry
            auto scene = _sceneManager->GetScene();
            if (scene) {
                auto registry = scene->GetEntityRegistry();
                if (registry) {
                    auto view = registry->view<DeltaTimeComponent>();
                    for (auto entity : view) {
                        auto& deltaTime = view.get<DeltaTimeComponent>(entity);
                        float fps = deltaTime.Delta > 0.0f ? 1.0f / deltaTime.Delta : 0.0f;

                        // Format FPS string
                        std::ostringstream fpsText;
                        fpsText << "FPS: " << std::fixed << std::setprecision(1) << fps;

                        _uiManager->UpdateElementText(_elementId, fpsText.str());
                        break; // Only need first delta time component
                    }
                }
            }
        }
    }

    void OnExit() const override {
        // No cleanup needed
    }
};
