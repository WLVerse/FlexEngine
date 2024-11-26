// WLVERSE [https://wlverse.web.app]
// flexprofiler.cpp
// 
// Profiler that allows tracking of any number of breakpoints and prints the execution time to an IMGUI window
//
// AUTHORS
// [100%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flexprofiler.h"
#include "windowsizes.h"

namespace FlexEngine
{
  bool Profiler::paused = false;
  std::map<std::string, std::chrono::microseconds> Profiler::execute_times;

  void Profiler::StartCounter(std::string const& name) 
  { 
    start_times[name] = std::chrono::high_resolution_clock::now(); 
  }

  void Profiler::EndCounter(std::string const& name)
  {
    if (start_times.find(name) != start_times.end())
    {
      if (!paused) // Only update when not paused
        execute_times[name] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_times[name]);
    }
  }

  // Create profiler window with IMGUI
  void Profiler::ShowProfilerWindow() 
  {
    SETPROFILERWINDOW
    ImGui::Begin("Profiler");

    // Calculating combined time
    long long combined_time = 0;
    for (const auto& [name, duration] : execute_times) { combined_time += duration.count(); }
    ImGui::Text("Combined execution time: %lld microseconds", combined_time);

    // Display each counter as percentage of execution time
    for (const auto& [name, duration] : execute_times)
    {
      ImGui::Text("Execution time of %s: %.2f%%", name.c_str(), (float)duration.count() / combined_time * 100);
    }

    if (ImGui::Button("Freeze numbers")) 
    { 
      paused = !paused;
    }

    ImGui::End();
  }
}
