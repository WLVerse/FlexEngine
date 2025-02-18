// WLVERSE [https://wlverse.web.app]
// physicslayer.cpp
//
// Physics layer for the editor. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"

#include <Physics/physicssystem.h>

namespace Editor
{

  void PhysicsLayer::OnAttach()
  {
  }

  void PhysicsLayer::OnDetach()
  {
  }

  void PhysicsLayer::Update()
  {
    Profiler::StartCounter("Physics Loop");

    FlexEngine::PhysicsSystem::UpdatePhysicsSystem();

    Profiler::EndCounter("Physics Loop");
  }

}