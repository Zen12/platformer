#pragma once

#include "../entity.hpp"
#include "../../ui/text/text_renderer_component.hpp"

class ShowFpsComponent final : public Component {

    std::weak_ptr<UiTextRendererComponent> _text;

public:
    explicit ShowFpsComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetText(const std::weak_ptr<UiTextRendererComponent> &text) {
        _text = text;
    }

    void Update(const float &deltaTime) override {
        if (const auto &text = _text.lock()) {
            const float fps = 1.0f / deltaTime;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(0) << fps;
            text->SetText(ss.str());
        }
    }
};
