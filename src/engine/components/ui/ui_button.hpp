#pragma once
#include "ui_interactable.hpp"
#include "../entity.hpp"
#include "../../render/camera.hpp"
#include "../renderering/camera_component.hpp"
#include "../transforms/rect_transform.hpp"


class UiButton final : public UiInteractable {
private:
    int _nextId = 0;
    std::vector<std::pair<int, std::function<void()>>> _callbacks;

public:
    explicit UiButton(const std::weak_ptr<Entity>& entity)
        : UiInteractable(entity) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {
        if (_state.IsPressUp) {
            for (const auto &callback: _callbacks) {
                callback.second();
            }
        }
    }

    int AddCallback(const std::function<void()>& callback);

    void RemoveCallback(int id);
};

