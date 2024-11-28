// WLVERSE [https://wlverse.web.app]
// flexprofiler.h
// 
// Profiler that allows tracking of any number of breakpoints and prints the execution time to an IMGUI window
// 
// Usage: Profiler profiler;
//        profiler.StartCounter("CounterName");
//        // Code to profile...
//        profiler.EndCounter("CounterName");
//        
//        // Show results
//        profiler.ShowProfilerWindow();
//
// AUTHORS
// [100%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <chrono>
#include <map>
#ifndef GAME
#include "imgui.h"
#endif
#include "flx_api.h"

namespace FlexEngine
{
  class __FLX_API Profiler
  {
    std::map<std::string, std::chrono::high_resolution_clock::time_point> start_times;
    static std::map<std::string, std::chrono::microseconds> execute_times;
    static bool paused;

  public:
    Profiler() = default;
    ~Profiler() = default;

    void StartCounter(std::string const& name);
    void EndCounter(std::string const& name);

    // Create profiler window with IMGUI
    void ShowProfilerWindow();
  };
}
