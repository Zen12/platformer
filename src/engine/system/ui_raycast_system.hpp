#pragma once
#include "input_system.hpp"

class UiInteractable;

class UiRaycastSystem {
protected:
    std::vector<std::weak_ptr<UiInteractable>> _interactables{};
    std::weak_ptr<InputSystem> _inputSystem;

public:
    virtual ~UiRaycastSystem() = default;

    void RegisterInteractable(const std::weak_ptr<UiInteractable>& interactable) noexcept {
        _interactables.push_back(interactable);
    }

    void SetInputSystem(const std::weak_ptr<InputSystem>& inputSystem) noexcept {
        _inputSystem = inputSystem;
    }

    virtual void UpdateState() = 0;

    void UnregisterInteractable(const std::weak_ptr<UiInteractable>& interactable) noexcept {
        _interactables.erase(
            std::remove_if(
                _interactables.begin(), _interactables.end(),
                [&](const std::weak_ptr<UiInteractable>& wptr) {
                    // remove if expired or matches
                    return wptr.expired() ||
                           (!wptr.owner_before(interactable) && !interactable.owner_before(wptr));
                }),
            _interactables.end()
        );
    }
};
