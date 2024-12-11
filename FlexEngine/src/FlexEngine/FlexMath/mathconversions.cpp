// WLVERSE [https://wlverse.web.app]
// mathconversions.cpp
// 
// Conversions between degrees and radians.
// 
// Values are taken from the glm library.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "mathconversions.h"

namespace FlexEngine
{

  float radians(float degrees)
  {
    return degrees * 0.01745329251994329576923690768489f;
  }

  double radians(double degrees)
  {
    return degrees * 0.01745329251994329576923690768489;
  }

  float degrees(float radians)
  {
    return radians * 57.295779513082320876798154814105f;
  }

  double degrees(double radians)
  {
    return radians * 57.295779513082320876798154814105;
  }

}
