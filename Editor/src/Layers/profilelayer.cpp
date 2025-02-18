// WLVERSE [https://wlverse.web.app]
// audiolayer.cpp
// 
// Audio implementation in the layer
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor
{
  void ProfilerLayer::OnAttach()
  {

  }

  void ProfilerLayer::OnDetach()
  {

  }

  void ProfilerLayer::Update()
  {
    Profiler::ShowProfilerWindow();
  }
}
