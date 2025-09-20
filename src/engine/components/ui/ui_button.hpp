#pragma once
#include "ui_interactable.hpp"
#include "../entity.hpp"
#include "../../render/camera.hpp"
#include "../renderering/camera_component.hpp"
#include "../transforms/rect_transform.hpp"


class UiButton final : public UiInteractable {
private:
    int _nextId = 0;
    std::vector<std::pair<int, std::function<void()>>> _onClickCallbacks;
    std::vector<std::pair<int, std::function<void()>>> _onHoverCallbacks;
    std::vector<std::pair<int, std::function<void()>>> _onDeselectCallbacks;

public:
    explicit UiButton(const std::weak_ptr<Entity>& entity)
        : UiInteractable(entity) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {
    }

    int AddOnClickCallback(const std::function<void()>& callback);
    int AddOnHoverCallback(const std::function<void()>& callback);
    int AddOnDeselectCallback(const std::function<void()>& callback);

    void RemoveOnClickCallback(int id);
    void RemoveOnHoverCallback(int id);
    void RemoveOnDeselectCallback(int id);

protected:
    void OnUpdatedState() override;
};

