#pragma once

#include "ui_button.hpp"
#include "../entity.hpp"

class OnClickSceneLoader final : public Component
{
private:
    std::weak_ptr<UiButton> _button;
    std::string _sceneGuid;
    std::weak_ptr<Scene> _scene;

    int _subscribeId{-1};

private:
    void OnClick() const noexcept {
        if (const auto scene = _scene.lock()) {
            scene->RequestToLoadScene(_sceneGuid);
        }
    }

public:
    explicit OnClickSceneLoader(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetScene(const std::weak_ptr<Scene> &scene) noexcept {
        _scene = scene;
    }

    void SetLoadScene(const std::string &value) noexcept {
        _sceneGuid = value;
    }

    void SetButton(const std::weak_ptr<UiButton> &button) noexcept {
        _button = button;
        if (const auto b = _button.lock()) {
            _subscribeId = b->AddCallback([this]() {
                this->OnClick();
            });
        }
    }

    ~OnClickSceneLoader() noexcept override {
        if (const auto b = _button.lock()) {
            b->RemoveCallback(_subscribeId);
        }
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }
};
