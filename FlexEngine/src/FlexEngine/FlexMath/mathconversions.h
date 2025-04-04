// WLVERSE [https://wlverse.web.app]
// mathconversions.h
//
// Conversions between degrees and radians. 
// 
// Values are taken from the glm library. 
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

  // Converts degrees to radians.
  __FLX_API float radians(float degrees);
  // Converts degrees to radians.
  __FLX_API double radians(double degrees);

  // Converts radians to degrees.
  __FLX_API float degrees(float radians);
  // Converts radians to degrees.
  __FLX_API double degrees(double radians);

}