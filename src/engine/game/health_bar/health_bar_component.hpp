#pragma once
#include "../../components/entity.hpp"
#include "../../ui/image/image_renderer_component.hpp"
#include "../health/health_component.hpp"


class HealthBarComponent final : public Component {

private:
    std::weak_ptr<UiImageRendererComponent> _image;
    std::weak_ptr<HealthComponent> _health;

public:

    explicit HealthBarComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

        if (const auto image = _image.lock()) {
            if (const auto health = _health.lock()) {
                image->SetFillAmount(health->GetPercentageHealth());
            }
        }
    }

    void SetFillBar(const std::weak_ptr<UiImageRendererComponent> &image) noexcept {
        _image = image;
    }

    void SetHealth(const std::weak_ptr<HealthComponent> &health) noexcept {
        _health = health;
    }
};