#pragma once
#include <vector>
#include <memory>

#include "../system/input_system.hpp"
#include "raycast_system.hpp"
#include "interactable.hpp"

class UiDesktopRaycast final : public UiRaycastSystem {
private:

    struct WeakPtrCompare {
        bool operator()(const std::weak_ptr<UiInteractableComponent>& a,
                        const std::weak_ptr<UiInteractableComponent>& b) const noexcept {
            return a.owner_before(b);
        }
    };

    std::set<std::weak_ptr<UiInteractableComponent>, WeakPtrCompare> _lastSelected;
public:
    void UpdateState() override {

        if (const auto input = _inputSystem.lock()) {

            const auto selectedState = UIInteractableState{
                .IsPressDown = input->IsMousePressing(MouseButton::Left),
                .IsPressUp = input->IsMouseUp(MouseButton::Left),
                .IsHovered = !input->IsMousePressing(MouseButton::Left) && !input->IsMouseUp(MouseButton::Left),
                .IsDeselected = false
            };

            constexpr auto notSelectedState = UIInteractableState{
                .IsPressDown = false,
                .IsPressUp = false,
                .IsHovered = false,
                .IsDeselected = false
            };

            constexpr auto deSelectedState = UIInteractableState{
                .IsPressDown = false,
                .IsPressUp = false,
                .IsHovered = false,
                .IsDeselected = true
            };


            for (const auto &interactable: _interactables) {
                if (const auto &inter = interactable.lock()) {
                    const auto mousePosition = input->GetMouseScreenSpace();
                    if (inter->IsPointInside(mousePosition)) {
                        inter->SetState(selectedState);
                        _lastSelected.insert(inter);
                    } else {
                        if (_lastSelected.erase(inter) > 0) {
                            inter->SetState(deSelectedState);
                        } else {
                            inter->SetState(notSelectedState);
                        }
                    }
                }
            }
        }
    }
};
