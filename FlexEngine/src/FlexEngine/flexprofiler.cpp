// WLVERSE [https://wlverse.web.app]
// flexprofiler.h
// 
// Profiler that allows tracking of any number of breakpoints and prints the execution time to an IMGUI window
//
// AUTHORS
// [100%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flexprofiler.h"
#include "flexlogger.h"

namespace FlexEngine
{
  bool Profiler::paused = false;
  long long Profiler::combined_time = 0;
  std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> Profiler::start_times; // auto erased by endcounter
  std::unordered_map<std::string, std::chrono::microseconds> Profiler::execute_times;

  void Profiler::StartCounter(std::string const& name) 
  { 
    if (paused) return;

    if (start_times.find(name) == start_times.end())
    {
      start_times[name] = std::chrono::high_resolution_clock::now();
    }
    else Log::Warning("Timer already exists with name: " + name + ". Please end the timer before starting a new one.");
  }

  void Profiler::EndCounter(std::string const& name)
  {
    if (paused) return;

    if (start_times.find(name) != start_times.end())
    {
      execute_times[name] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_times[name]);
      start_times.erase(name);
    }
    else Log::Warning("No timer exists with name: " + name + ". Make sure you start a timer before calling end");
  }

  // Create profiler window with IMGUI
  void Profiler::ShowProfilerWindow() 
  {
    #ifndef GAME
    ImGui::Begin("Profiler");

    if (ImGui::Button(paused ? "Unfreeze numbers" : "Freeze numbers"))
    { 
      paused = !paused;
    }
    
    if (!paused)
    {
      // Recalculate accumulated time if not paused
      combined_time = 0;
      for (const auto& [name, duration] : execute_times) { combined_time += duration.count(); }
    }

    ImGui::Text("Combined execution time: %lld microseconds", combined_time);

    // Display each counter as percentage of execution time
    if (combined_time > 0) // wary of edge case where combined_time is 0
    {
      for (const auto& [name, duration] : execute_times)
      {
        ImGui::Text("Execution time of %s: %.2f%%", name.c_str(), (float)duration.count() / combined_time * 100);
      }
    }

    // Clear every frame as long as not frozen
    if (!paused) execute_times.clear();

    ImGui::End();
    #endif
  }
}
