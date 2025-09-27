#pragma once
#include "../../components/entity.hpp"
#include "../../components/renderering/spine_renderer.hpp"


class IdleCharacter final : public Component {
private:
    std::weak_ptr<SpineRenderer> _renderer;
public:
    explicit IdleCharacter(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }

    void SetAnimation(const std::string &animation) const {
        if (const auto renderer = _renderer.lock()) {
            renderer->SetAnimation(0, animation, true, false);
        }
    }

    void SetSpineRenderer(const std::weak_ptr<SpineRenderer> &spineRenderer) noexcept {
        _renderer = spineRenderer;
    }
};