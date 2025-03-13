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

#include <chrono>
#include <unordered_map>
#ifndef GAME
#include "imgui.h"
#endif
#include "flx_api.h"

namespace FlexEngine
{
  // Profiler that allows tracking of any number of breakpoints and prints the execution time to an IMGUI window
  // For optimisation purposes, this class will not check if you call start without an end. If this happens, the profiler will not show the time for that counter.
  // 
  // Usage: Profiler profiler;
  //        profiler.StartCounter("CounterName");
  //        // Code to profile...
  //        profiler.EndCounter("CounterName");
  //        
  //        // Show results
  //        profiler.ShowProfilerWindow();
  class __FLX_API Profiler
  {
    static Profiler profiler;
    static std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> start_times;
    static std::unordered_map<std::string, std::chrono::microseconds> execute_times;
    static long long combined_time;
    static bool paused;

  public:
    Profiler() = default;
    ~Profiler() = default;

    static void StartCounter(std::string const& name);
    static void EndCounter(std::string const& name);

    // Create profiler window with IMGUI
    static void ShowProfilerWindow();
  };
}
