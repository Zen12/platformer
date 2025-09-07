#pragma once
#include "../../components/entity.hpp"
#include "../../components/ui_image_renderer.hpp"


class HealthBarComponent final : public Component {
private:
    std::weak_ptr<UiImageRenderer> _image;
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

    void SetFillBar(const std::weak_ptr<UiImageRenderer> &image) noexcept {
        _image = image;
    }

    void SetHealth(const std::weak_ptr<HealthComponent> &health) noexcept {
        _health = health;
    }


};