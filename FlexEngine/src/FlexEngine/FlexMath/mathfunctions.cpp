// WLVERSE [https://wlverse.web.app]
// mathfunctions.cpp
// 
// Mathematical functions.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "mathfunctions.h"

namespace FlexEngine
{
  namespace FlexMath
  {
    #pragma region Template Definition
    template <typename T>
    T Clamp(T value, T min, T max)
    {
      return value < min ? min : (value > max ? max : value);
    }


    template <typename T>
    __FLX_API T Lerp(const T& a, const T& b, float t)
    {
        // Clamp t to ensure it remains in the range [0,1]
        t = Clamp(t, 0.0f, 1.0f);
        return a + (b - a) * t;
    }
    
    #pragma endregion

    #pragma region Explicit instantiation.
    // Explicit instantiation.
    template __FLX_API int Clamp<int>(int value, int min, int max);
    template __FLX_API float Clamp<float>(float value, float min, float max);
    template __FLX_API double Clamp<double>(double value, double min, double max);

    template __FLX_API int Lerp<int>(const int& a, const int& b, float t);
    template __FLX_API float Lerp<float>(const float& a, const float& b, float t);
    template __FLX_API double Lerp<double>(const double& a, const double& b, float t);
    #pragma endregion
  }
}
