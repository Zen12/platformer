#pragma once

#include "../../components/entity.hpp"
#include "../text/text_renderer_component.hpp"

#include <sstream>
#include <iomanip>

class ShowFpsComponent final : public Component
{
private:
    std::weak_ptr<UiTextRendererComponent> _text{};

public:
    explicit ShowFpsComponent(const std::weak_ptr<Entity> &entity);

    void SetText(const std::weak_ptr<UiTextRendererComponent> &text) { _text = text; }

    void Update(const float &deltaTime) override;
};