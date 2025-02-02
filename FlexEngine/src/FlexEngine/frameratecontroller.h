// WLVERSE [https://wlverse.web.app]
// frameratecontroller.h
//
// Rough implementation of a framerate controller.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <chrono>

namespace FlexEngine
{

  class __FLX_API FramerateController
  {
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_time = std::chrono::high_resolution_clock::now();
    float m_delta_time = 0.0f;
    float m_frame_time_accumulator = 0.0f;
    unsigned int m_fps = 0;
    unsigned int m_frame_counter = 0;
    unsigned int m_target_fps = 60;

    // Number of fixed steps to process
    unsigned int m_number_of_steps = 0;

  public:
    void BeginFrame();
    void EndFrame();

#pragma region Getter/Setter Functions

    float GetDeltaTime() const;
    float GetFixedDeltaTime() const;

    unsigned int GetFPS() const;
    unsigned int GetNumberOfSteps() const;

    void SetTargetFPS(unsigned int fps = 0);

#pragma endregion
  };

} // namespace FlexEngine
