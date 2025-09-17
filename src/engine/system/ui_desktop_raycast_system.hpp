#pragma once
#include <vector>
#include <memory>

#include "input_system.hpp"
#include "ui_raycast_system.hpp"
#include "../components/ui/ui_interactable.hpp"

class UiDesktopRaycastSystem final : public UiRaycastSystem {
private:

public:
    void UpdateState() override {

        if (const auto input = _inputSystem.lock()) {

            const auto selectedState = UIInteractableState{
                .IsPressDown = input->IsMousePressing(MouseButton::Left),
                .IsPressUp = input->IsMouseUp(MouseButton::Left),
                .IsHovered = !input->IsMousePressing(MouseButton::Left) && !input->IsMouseUp(MouseButton::Left),
            };

            constexpr auto notSelectedState = UIInteractableState{
                .IsPressDown = false,
                .IsPressUp = false,
                .IsHovered = false,
            };


            for (const auto &interactable: _interactables) {
                if (const auto &inter = interactable.lock()) {
                    const auto mousePosition = input->GetMouseScreenSpace();
                    if (inter->IsPointInside(mousePosition)) {
                        inter->SetState(selectedState);
                    } else {
                        inter->SetState(notSelectedState);
                    }
                }
            }
        }
    }
};
