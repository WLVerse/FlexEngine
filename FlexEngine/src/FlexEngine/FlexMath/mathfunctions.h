// WLVERSE [https://wlverse.web.app]
// mathfunctions.h
//
// Mathematical functions. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

namespace FlexEngine
{
  namespace FlexMath
  {

    // Clamps a value between a minimum and maximum.
    template <typename T>
    __FLX_API T Clamp(T value, T min, T max);

    // A generic linear interpolation function
    template <typename T>
    __FLX_API T Lerp(const T& a, const T& b, float t);


  }
}