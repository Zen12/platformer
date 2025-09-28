#include "spine_data.hpp"


/// Set the default extension used for memory allocations and file I/O
spine::SpineExtension *spine::getDefaultExtension() {
    return new spine::DefaultSpineExtension();
}


void SpineData::SetAnimation(const size_t &index, const std::string &name, const bool &isLoop, const bool &isReverse) const {
    const auto track = _animationState->setAnimation(index, name.c_str(), isLoop);
    track->setReverse(isReverse);
}

SpineData::SpineData(std::shared_ptr<spine::SkeletonData> skeletonData,
    std::shared_ptr<spine::AnimationStateData> stateData, std::shared_ptr<spine::Skeleton> skeleton,
    std::shared_ptr<spine::AnimationState> animationState, std::string moveAnimationName, std::string jumpAnimationName,
    std::string hitAnimationName, std::string idleAnimationName):
    _skeleton(std::move(skeleton)),
    _animationState(std::move(animationState)),
    _skeletonData(std::move(skeletonData)),
    _stateData(std::move(stateData)),
    MoveAnimationName(moveAnimationName),
    JumpAnimationName(jumpAnimationName),
    HitAnimationName(hitAnimationName),
    IdleAnimationName(idleAnimationName){
}

void SpineData::AppendAnimation(const size_t &index, const std::string &name, const bool &isLoop) const {
    _animationState->addAnimation(index, name.c_str(), isLoop, 0.0);
}

void SpineData::SetTransition(const std::string &anim1, const std::string &anim2, const float &time) const {
    _stateData->setMix(anim1.c_str(), anim2.c_str(), time);
}
