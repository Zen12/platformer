#pragma once

#include "action.hpp"
#include "../../../scene/scene_manager.hpp"
#include "../../../esc/health/health_component.hpp"
#include <memory>
#include <string>
#include <unordered_map>

struct HealthCheckAction final : public Action {

private:
    std::string _triggerName;
    std::shared_ptr<SceneManager> _sceneManager;
    std::unordered_map<std::string, bool>& _triggers;

public:
    HealthCheckAction(std::string triggerName, std::shared_ptr<SceneManager> sceneManager, std::unordered_map<std::string, bool>& triggers)
        : _triggerName(std::move(triggerName)), _sceneManager(std::move(sceneManager)), _triggers(triggers) {}

    void OnEnter() const override {
    }

    void OnUpdate() const override {
        if (!_sceneManager) return;

        auto scene = _sceneManager->GetScene();
        if (!scene) return;

        auto registry = scene->GetEntityRegistry();
        if (!registry) return;

        auto view = registry->view<HealthComponent>();
        for (auto entity : view) {
            auto& health = view.get<HealthComponent>(entity);
            if (health.GetCurrentHealth() <= 0.0f) {
                _triggers[_triggerName] = true;
                return;
            }
        }
    }

    void OnExit() const override {
    }
};
