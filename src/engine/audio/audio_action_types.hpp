#pragma once
#include "play_sound_action.hpp"
#include "play_sound_repeated_action.hpp"
#include "mute_audio_action.hpp"
#include "../fsm/type_list.hpp"

using AudioActionTypes = fsm::TypeList<
    PlaySoundAction,
    PlaySoundRepeatedAction,
    MuteAudioAction
>;
