#pragma once

#include "spine/Atlas.h"
#include "spine/Skeleton.h"
#include <spine/spine.h>

#include "stb_image.h"
#include "../mesh.hpp"
#include "../shader.hpp"
#include "GL/glew.h"


/// A vertex of a mesh generated from a Spine skeleton
struct vertex_t {
    float x, y;
    uint32_t color;
    float u, v;
    uint32_t darkColor;
};


class GlTextureLoader : public spine::TextureLoader {
public:
    void load(spine::AtlasPage &page, const spine::String &path);
    void unload(void *texture);
};


class SpineData
{
private:
    std::shared_ptr<spine::Skeleton> _skeleton;
    std::shared_ptr<spine::AnimationState> _animationState;

public:
    SpineData(
        const std::shared_ptr<spine::Skeleton> &skeleton,
        const std::shared_ptr<spine::AnimationState> &animationState,
        const float& scaleFactor):
        _skeleton(skeleton),
        _animationState(animationState)
    {
        // We use a y-down coordinate system, see renderer_set_viewport_size()
        //_skeleton->setPosition(0, 0);
        _skeleton->setScaleX(scaleFactor);
        _skeleton->setScaleY(-scaleFactor);

        //_animationState->setAnimation(0, "portal", true);
        _animationState->addAnimation(0, "run", true, 0);

    }

    [[nodiscard]] std::weak_ptr<spine::Skeleton> GetSkeleton() const { return _skeleton; }

    [[nodiscard]] std::weak_ptr<spine::AnimationState> GetAnimationState() const { return _animationState; }

    void SetScale(const glm::vec2 &scale) const noexcept{
        _skeleton->setScaleX(scale.x);
        _skeleton->setScaleY(scale.y);
    }

    void SetScale(const float &scale) const noexcept {
        _skeleton->setScaleX(scale);
        _skeleton->setScaleY(scale);
    }
};
