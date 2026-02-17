#pragma once

#include "action.hpp"
#include "ui_manager.hpp"
#include "scene_manager.hpp"
#include "esc/health/health_component.hpp"
#include "esc/transform/transform_component.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

struct HealthDisplayAction final : public Action {

private:
    std::string _elementId;
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<SceneManager> _sceneManager;

public:
    HealthDisplayAction(std::string elementId, std::shared_ptr<UIManager> uiManager, std::shared_ptr<SceneManager> sceneManager)
        : _elementId(std::move(elementId)), _uiManager(std::move(uiManager)), _sceneManager(std::move(sceneManager)) {}

    void OnEnter() const override {
    }

    void OnUpdate() const override {
        if (_uiManager && _sceneManager && !_elementId.empty()) {
            auto scene = _sceneManager->GetScene();
            if (scene) {
                auto registry = scene->GetEntityRegistry();
                if (registry) {
                    auto view = registry->view<HealthComponent>();
                    for (auto entity : view) {
                        auto& health = view.get<HealthComponent>(entity);

                        std::ostringstream healthText;
                        healthText << "HP: " << std::fixed << std::setprecision(0)
                                   << health.GetCurrentHealth() << "/" << health.GetMaxHealth();

                        _uiManager->UpdateElementText(_elementId, healthText.str());
                        break;
                    }
                }
            }
        }
    }

    void OnExit() const override {
    }
};
