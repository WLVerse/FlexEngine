// WLVERSE [https://wlverse.web.app]
// EngineComponents.cpp
// 
// Just the refl register for the Audio component.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "EngineComponents.h"

namespace FlexEngine
{
  FLX_REFL_REGISTER_START(Position)
    FLX_REFL_REGISTER_PROPERTY(position)
    FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Scale)
    FLX_REFL_REGISTER_PROPERTY(scale)
    FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Rotation)
    FLX_REFL_REGISTER_PROPERTY(rotation)
    FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Transform)
    FLX_REFL_REGISTER_PROPERTY(is_dirty)
    FLX_REFL_REGISTER_PROPERTY(is_active)
    FLX_REFL_REGISTER_PROPERTY(transform)
    FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(ZIndex)
    FLX_REFL_REGISTER_PROPERTY(z)
    FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Parent)
    FLX_REFL_REGISTER_PROPERTY(parent)
    FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Audio)
    FLX_REFL_REGISTER_PROPERTY(should_play)
    FLX_REFL_REGISTER_PROPERTY(is_looping)
    FLX_REFL_REGISTER_PROPERTY(audio_file) 
    FLX_REFL_REGISTER_END;
}