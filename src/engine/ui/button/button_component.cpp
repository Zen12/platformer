#include "button_component.hpp"

UiButtonComponent::UiButtonComponent(const std::weak_ptr<Entity>& entity)
    : UiInteractableComponent(entity)
{
}

void UiButtonComponent::Update([[maybe_unused]] const float &deltaTime)
{
}

int UiButtonComponent::AddOnClickCallback(const std::function<void()> &callback)
{
    int id = _nextId++;
    _onClickCallbacks.emplace_back(id, callback);
    return id;
}

int UiButtonComponent::AddOnHoverCallback(const std::function<void()> &callback)
{
    int id = _nextId++;
    _onHoverCallbacks.emplace_back(id, callback);
    return id;
}

int UiButtonComponent::AddOnDeselectCallback(const std::function<void()> &callback)
{
    int id = _nextId++;
    _onDeselectCallbacks.emplace_back(id, callback);
    return id;
}

void UiButtonComponent::RemoveOnClickCallback(int id)
{
    _onClickCallbacks.erase(
        std::remove_if(_onClickCallbacks.begin(), _onClickCallbacks.end(),
                       [id](const auto& pair) { return pair.first == id; }),
        _onClickCallbacks.end());
}

void UiButtonComponent::RemoveOnHoverCallback(int id)
{
    _onHoverCallbacks.erase(
        std::remove_if(_onHoverCallbacks.begin(), _onHoverCallbacks.end(),
                       [id](const auto& pair) { return pair.first == id; }),
        _onHoverCallbacks.end());
}

void UiButtonComponent::RemoveOnDeselectCallback(int id)
{
    _onDeselectCallbacks.erase(
        std::remove_if(_onDeselectCallbacks.begin(), _onDeselectCallbacks.end(),
                       [id](const auto& pair) { return pair.first == id; }),
        _onDeselectCallbacks.end());
}

void UiButtonComponent::OnUpdatedState()
{
    if (_state.IsPressUp) {
        for (const auto &callback: _onClickCallbacks) {
            callback.second();
        }
    } else if (_state.IsHovered) {
        for (const auto &callback: _onHoverCallbacks) {
            callback.second();
        }
    } else if (_state.IsDeselected) {
        for (const auto &callback: _onDeselectCallbacks) {
            callback.second();
        }
    }
}



