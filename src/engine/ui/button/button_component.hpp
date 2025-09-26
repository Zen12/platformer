#pragma once

#include "../interactable.hpp"
#include "../../components/entity.hpp"
#include "../../components/transforms/rect_transform.hpp"

#include <functional>
#include <vector>

class UiButtonComponent final : public UiInteractableComponent
{
private:
    int _nextId{0};
    std::vector<std::pair<int, std::function<void()>>> _onClickCallbacks{};
    std::vector<std::pair<int, std::function<void()>>> _onHoverCallbacks{};
    std::vector<std::pair<int, std::function<void()>>> _onDeselectCallbacks{};

public:
    explicit UiButtonComponent(const std::weak_ptr<Entity>& entity);

    int AddOnClickCallback(const std::function<void()>& callback);
    int AddOnHoverCallback(const std::function<void()>& callback);
    int AddOnDeselectCallback(const std::function<void()>& callback);

    void RemoveOnClickCallback(int id);
    void RemoveOnHoverCallback(int id);
    void RemoveOnDeselectCallback(int id);

    void Update([[maybe_unused]] const float &deltaTime) override;

protected:
    void OnUpdatedState() override;
};

