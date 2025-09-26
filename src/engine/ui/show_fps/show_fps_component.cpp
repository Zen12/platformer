#include "show_fps_component.hpp"

ShowFpsComponent::ShowFpsComponent(const std::weak_ptr<Entity> &entity)
    : Component(entity)
{
}

void ShowFpsComponent::Update(const float &deltaTime)
{
    if (const auto &text = _text.lock()) {
        const float fps = 1.0f / deltaTime;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << fps;
        text->SetText(ss.str());
    }
}