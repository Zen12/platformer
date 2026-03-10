#include "scene.hpp"

std::weak_ptr<InputSystem> Scene::GetInputSystem() const noexcept {
    return _inputSystem;
}

std::weak_ptr<Window> Scene::GetWindow() const noexcept {
    return _window;
}
