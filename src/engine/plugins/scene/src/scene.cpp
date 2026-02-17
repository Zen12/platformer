#include "scene.hpp"

std::weak_ptr<AssetManager> Scene::GetAssetManager() const noexcept {
    return _assetManager;
}

std::weak_ptr<InputSystem> Scene::GetInputSystem() const noexcept {
    return _inputSystem;
}

std::weak_ptr<Window> Scene::GetWindow() const noexcept {
    return _window;
}
