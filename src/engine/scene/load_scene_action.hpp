#pragma once

#include "../fsm/node/action/action.hpp"
#include "scene_manager.hpp"
#include "scene_asset.hpp"

struct LoadSceneAction final : public Action {

private:
    std::string _sceneGuid;
    std::shared_ptr<SceneManager> _sceneManager;

public:
    LoadSceneAction(std::string sceneGuid, std::shared_ptr<SceneManager> sceneManager)
        : _sceneGuid(std::move(sceneGuid)), _sceneManager(std::move(sceneManager)) {}

    void OnEnter() const override {
        if (_sceneManager) {
            _sceneManager->LoadSceneByGuid(_sceneGuid);
        }
    }

    void OnUpdate() const override {
        if (_sceneManager) {
            _sceneManager->Update();
        }
    }

    void OnExit() const override {
        // Scene cleanup if needed
    }
};
