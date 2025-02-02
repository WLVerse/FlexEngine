// WLVERSE [https://wlverse.web.app]
// frameratecontroller.cpp
//
// Rough implementation of a framerate controller.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "frameratecontroller.h"

#include <algorithm> // For std::max
#include <thread>

namespace FlexEngine
{

  void FramerateController::BeginFrame()
  {
    // Calculate delta time
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_diff = current_time - m_last_time;
    m_delta_time = time_diff.count();
    m_last_time = current_time;

    // Calculate FPS
    m_frame_time_accumulator += m_delta_time;
    m_frame_counter++;
    if (m_frame_time_accumulator >= 1.0f)
    {
      m_fps = m_frame_counter;
      m_frame_counter = 0;
      m_frame_time_accumulator -= 1.0f;
    }
  }

  void FramerateController::EndFrame()
  {
    if (m_target_fps == 0) return;

    m_number_of_steps = 0;
    float target_frame_time = 1.0f / m_target_fps; // Time per frame in seconds

    // If the frame is taking longer than expected, accumulate time and process frames
    m_frame_time_accumulator += m_delta_time;

    // Loop if the accumulated time exceeds the target frame time
    while (m_frame_time_accumulator >= target_frame_time)
    {
      m_frame_time_accumulator -= target_frame_time;
      m_number_of_steps++;
    }

    // Clamp accumulator to avoid negative values due to precision errors
    m_frame_time_accumulator = std::max(0.0f, m_frame_time_accumulator);

    // Calculate remaining time to sleep
    float remaining_time = target_frame_time - m_frame_time_accumulator;
    if (remaining_time > 0.0f)
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remaining_time * 1000.0f + 0.5f)));
  }

  float FramerateController::GetDeltaTime() const
  {
    return m_delta_time;
  }

  float FramerateController::GetFixedDeltaTime() const
  {
    return (m_target_fps == 0) ? GetDeltaTime() : (1.0f / m_target_fps);
  }

  unsigned int FramerateController::GetFPS() const
  {
    return m_fps;
  }

  unsigned int FramerateController::GetNumberOfSteps() const
  {
    return m_number_of_steps;
  }

  void FramerateController::SetTargetFPS(unsigned int fps)
  {
    m_target_fps = fps;
  }

} // namespace FlexEngine
