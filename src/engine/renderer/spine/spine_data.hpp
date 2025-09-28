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
    const std::string MoveAnimationName;
    const std::string JumpAnimationName;
    const std::string HitAnimationName;
    const std::string IdleAnimationName;

public:
    SpineData(
        std::shared_ptr<spine::SkeletonData> skeletonData,
        std::shared_ptr<spine::AnimationStateData> stateData,
        std::shared_ptr<spine::Skeleton> skeleton,
        std::shared_ptr<spine::AnimationState> animationState,
        std::string moveAnimationName,
        std::string jumpAnimationName,
        std::string hitAnimationName,
        std::string idleAnimationName);

    void AppendAnimation(const size_t &index, const std::string &name, const bool &isLoop) const;
    void SetAnimation(const size_t &index, const std::string &name, const bool &isLoop, const bool &isReverse) const;

    void SetTransition(const std::string &anim1, const std::string &anim2, const float &time) const;

    [[nodiscard]] std::weak_ptr<spine::Skeleton> GetSkeleton() const noexcept { return _skeleton; }

    [[nodiscard]] std::weak_ptr<spine::AnimationState> GetAnimationState() const noexcept { return _animationState; }

};
