// WLVERSE [https://wlverse.web.app]
// frameratecontroller.cpp
// 
// Rough implementation of the framerate controller.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "frameratecontroller.h"

#include <thread>

namespace FlexEngine
{

  void FramerateController::BeginFrame()
  {
    // Calculate delta time
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::chrono::seconds::period> time_diff = current_time - m_last_time;
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

    // Limit FPS
    if (m_frame_counter >= m_target_fps)
    {
      // Calculate remaining time and sleep if needed
      float target_frame_time = 1.0f / m_target_fps;
      float remaining_time = target_frame_time * 1000.0f - m_delta_time;
      if (remaining_time > 0.0f)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(remaining_time + 0.5f)));
      }
    }
  }

  float FramerateController::GetDeltaTime() const
  {
    return m_delta_time;
  }

  unsigned int FramerateController::GetFPS() const
  {
    return m_fps;
  }

  void FramerateController::SetTargetFPS(unsigned int fps)
  {
    m_target_fps = fps;
  }

}
