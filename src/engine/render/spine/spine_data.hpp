#pragma once

#include "spine/Atlas.h"
#include "spine/Skeleton.h"
#include <spine/spine.h>
#include "stb_image.h"
#include "GL/glew.h"

#include "../shader.hpp"


class GlTextureLoader : public spine::TextureLoader {
public:
    void load(spine::AtlasPage &page, const spine::String &path);
    void unload(void *texture);
};


class SpineData
{
private:
    const std::shared_ptr<spine::Skeleton> _skeleton;
    const std::shared_ptr<spine::AnimationState> _animationState;

public:
    SpineData(
        const std::shared_ptr<spine::Skeleton> &skeleton,
        const std::shared_ptr<spine::AnimationState> &animationState):
        _skeleton(skeleton),
        _animationState(animationState)
    {
        // hardcoded
        SetAnimation("run");
    }

    void SetAnimation(const std::string &name) const {
        _animationState->addAnimation(0, name.c_str(), true, 0);
    }

    [[nodiscard]] std::weak_ptr<spine::Skeleton> GetSkeleton() const noexcept { return _skeleton; }

    [[nodiscard]] std::weak_ptr<spine::AnimationState> GetAnimationState() const noexcept { return _animationState; }

    void SetScale(const glm::vec2 &scale) const noexcept {
        _skeleton->setScaleX(scale.x);
        _skeleton->setScaleY(scale.y);
    }

    void SetScale(const float &scale) const noexcept {
        _skeleton->setScaleX(scale);
        _skeleton->setScaleY(scale);
    }
};
