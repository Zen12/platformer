#pragma once

#include "action.hpp"
#include "../../../renderer/ui_manager.hpp"
#include "../../../scene/scene_manager.hpp"
#include "../../../esc/health/health_component.hpp"
#include <memory>
#include <string>
#include <iostream>

struct HealthBarAction final : public Action {

private:
    std::string _elementId;
    std::shared_ptr<UIManager> _uiManager;
    std::shared_ptr<SceneManager> _sceneManager;
    mutable float _lastPercent = -1.0f;

public:
    HealthBarAction(std::string elementId, std::shared_ptr<UIManager> uiManager, std::shared_ptr<SceneManager> sceneManager)
        : _elementId(std::move(elementId)), _uiManager(std::move(uiManager)), _sceneManager(std::move(sceneManager)) {}

    void OnEnter() const override {
    }

    void OnUpdate() const override {
        if (!_uiManager || !_sceneManager || _elementId.empty()) return;

        auto scene = _sceneManager->GetScene();
        if (!scene) return;

        auto registry = scene->GetEntityRegistry();
        if (!registry) return;

        auto view = registry->view<HealthComponent>();
        for (auto entity : view) {
            auto& health = view.get<HealthComponent>(entity);
            float current = health.GetCurrentHealth();
            float max = health.GetMaxHealth();

            // Only update when health changes
            if (std::abs(current - _lastPercent) > 0.1f) {
                _uiManager->UpdateProgressValue(_elementId, current, max);
                _lastPercent = current;
            }
            break;
        }
    }

    void OnExit() const override {
    }
};
