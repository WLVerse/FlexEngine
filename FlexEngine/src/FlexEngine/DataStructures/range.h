// WLVERSE [https://wlverse.web.app]
// range.h
//
// Data structure for a range of values. 
// Use Get() to generate random values within the range. 
// The number that is generated is inclusive of the min and max values. 
// Can also be used to store a range of values by calling Min() and Max(). 
// 
// Usage: int rand = Range<int>(3, 5).Get(); // gets a random number from 3 to 5 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <cstdlib> // rand()

namespace FlexEngine
{

  // Data structure for a range of values.
  // Use Get() to generate random values within the range.
  // The number that is generated is inclusive of the min and max values.
  // Can also be used to store a range of values by calling Min() and Max().
  template <typename T>
  struct Range
  {
    T min, max;

    Range(T value = 0);
    Range(T _min, T _max);

    // Returns a random value within the range
    // The number that is generated is inclusive of the min and max values.
    T Get() const;
  };

  #pragma region Definitions

  template <typename T>
  Range<T>::Range(T value)
    : min(value), max(value)
  {
  }

  template <typename T>
  Range<T>::Range(T _min, T _max)
  {
    // make sure min is less than max
    if (_min > _max) { min = _max; max = _min; }
    else { min = _min; max = _max; }
  }

  template <typename T>
  T Range<T>::Get() const
  {
    // guard: same value
    if (min == max) return min;

    if constexpr (std::is_integral<T>() || std::is_enum<T>())
    {
      return static_cast<T>(min + rand() % (max - min + 1));
    }
    else
    {
      return static_cast<T>(min + (max - min) * (rand() / static_cast<double>(RAND_MAX)));
    }
  }

  #pragma endregion

}