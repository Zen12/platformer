#pragma once
#include "type_list.hpp"

#include "ui/ui_action_types.hpp"
#include "scene_action_types.hpp"
#include "audio_action_types.hpp"
#include "node/action/animation_action_types.hpp"
#include "node/action/core_action_types.hpp"
#include "node/action/debug_action_types.hpp"

namespace fsm {

using AllActionTypes = Concat_t<
    UiActionTypes,
    SceneActionTypes,
    AudioActionTypes,
    AnimationActionTypes,
    CoreActionTypes,
    DebugActionTypes
>;

using AllActionVariants = ToVariant_t<AllActionTypes>;

}
