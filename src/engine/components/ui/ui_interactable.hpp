#pragma once
#include "../entity.hpp"
#include "../../system/ui_raycast_system.hpp"
#include "../transforms/rect_transform.hpp"


struct UIInteractableState {
    bool IsPressDown{};
    bool IsPressUp{};
    bool IsHovered{};
};

class UiInteractable : public Component {
private:
    std::weak_ptr<UiRaycastSystem> _uiRaycastSystem;
    std::weak_ptr<UiInteractable> _self;
protected:
    std::weak_ptr<RectTransform> _rectTransform;
    UIInteractableState _state{};

public:
    explicit UiInteractable(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    ~UiInteractable() override {
        if (const auto ray = _uiRaycastSystem.lock()) {
            ray->UnregisterInteractable(_self);
        }
    }

    void SetUiRaycastSystem(const std::weak_ptr<UiRaycastSystem> &uiRaycastSystem) {
        _uiRaycastSystem = uiRaycastSystem;
        if (const auto ray = _uiRaycastSystem.lock()) {
            ray->RegisterInteractable(_self);
        }
    }

    void Update(const float &deltaTime) override = 0;

    void SetState(const UIInteractableState &state) noexcept {
        _state = state;
    }

    void SetSelf(const std::weak_ptr<UiInteractable> &self) noexcept {
        _self = self;
    }

    void SetRectTransform(const std::weak_ptr<RectTransform> &rectTransform) noexcept {
        _rectTransform = rectTransform;
    }

    bool IsPointInside(const glm::vec2 &position) const noexcept {
        if (const auto rect = _rectTransform.lock()) {
            const auto model = rect->GetModel();
            const glm::vec3 pos = glm::vec3(model[3]);

            glm::vec2 scale;
            scale.x = glm::length(glm::vec3(model[0])); // column 0
            scale.y = glm::length(glm::vec3(model[1])); // column 1

            const auto lowerLeft = glm::vec2(pos.x, pos.y);
            const auto topRight = glm::vec2(pos.x + scale.x, pos.y + scale.y);
            if (position.x > lowerLeft.x && position.x < topRight.x) {
                if (position.y > lowerLeft.y && position.y < topRight.y) {
                    return true;
                }
            }
        }
        return false;
    }

};
