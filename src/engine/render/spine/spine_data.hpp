#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <spine/spine.h>

class SpineData {
private:
    const std::shared_ptr<spine::Skeleton> _skeleton;
    const std::shared_ptr<spine::AnimationState> _animationState;

    const std::shared_ptr<spine::SkeletonData> _skeletonData;
    const std::shared_ptr<spine::AnimationStateData> _stateData;

public:
    SpineData(
        std::shared_ptr<spine::SkeletonData> skeletonData,
        std::shared_ptr<spine::AnimationStateData> stateData,
        std::shared_ptr<spine::Skeleton> skeleton,
        std::shared_ptr<spine::AnimationState> animationState)
        :
          _skeleton(std::move(skeleton)),
          _animationState(std::move(animationState)),
          _skeletonData(std::move(skeletonData)),
          _stateData(std::move(stateData))
    {}

    void SetAnimation(const std::string &name) const {
        _animationState->setAnimation(0, name.c_str(), true);
    }

    void SetTransition(const std::string &anim1, const std::string &anim2, const float &time) const {
        _stateData->setMix(anim1.c_str(), anim2.c_str(), time);
    }

    [[nodiscard]] std::weak_ptr<spine::Skeleton> GetSkeleton() const noexcept { return _skeleton; }

    [[nodiscard]] std::weak_ptr<spine::AnimationState> GetAnimationState() const noexcept { return _animationState; }


};
