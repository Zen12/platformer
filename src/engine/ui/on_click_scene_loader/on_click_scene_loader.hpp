#pragma once

#include "../button/button_component.hpp"
#include "../../entity/entity.hpp"
#include "../../scene/scene.hpp"

class OnClickSceneLoader final : public Component
{
private:
    std::weak_ptr<UiButtonComponent> _button{};
    std::string _sceneGuid{};
    std::weak_ptr<Scene> _scene{};
    int _subscribeId{-1};

public:
    explicit OnClickSceneLoader(const std::weak_ptr<Entity> &entity);

    void SetScene(const std::weak_ptr<Scene> &scene) { _scene = scene; }
    void SetLoadScene(const std::string &value) { _sceneGuid = value; }
    void SetButton(const std::weak_ptr<UiButtonComponent> &button);

    void Update([[maybe_unused]] const float &deltaTime) override;

    ~OnClickSceneLoader() noexcept override;

private:
    void OnClick() const;
};