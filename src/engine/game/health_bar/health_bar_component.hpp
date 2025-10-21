#pragma once
#include "../../entity/entity.hpp"
#include "../../ui/image/image_renderer_component.hpp"
#include "../health/health_component.hpp"


class HealthBarComponent {

private:
    UiImageRendererComponent *_image;
    HealthComponent *_health;

public:

    HealthBarComponent(HealthComponent *health, UiImageRendererComponent* image) {
        _health = health;
        _image = image;
    }

    void Update() const {
        _image->SetFillAmount(_health->GetPercentageHealth());
    }
};