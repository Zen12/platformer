#include "on_click_scene_loader.hpp"

OnClickSceneLoader::OnClickSceneLoader(const std::weak_ptr<Entity> &entity)
    : Component(entity)
{
}

void OnClickSceneLoader::Update([[maybe_unused]] const float &deltaTime)
{
}

void OnClickSceneLoader::SetButton(const std::weak_ptr<UiButtonComponent> &button)
{
    _button = button;
    if (const auto b = _button.lock()) {
        _subscribeId = b->AddOnClickCallback([this]() {
            this->OnClick();
        });
    }
}

OnClickSceneLoader::~OnClickSceneLoader() noexcept
{
    if (const auto b = _button.lock()) {
        b->RemoveOnClickCallback(_subscribeId);
    }
}

void OnClickSceneLoader::OnClick() const
{
    if (const auto scene = _scene.lock()) {
        scene->RequestToLoadScene(_sceneGuid);
    }
}