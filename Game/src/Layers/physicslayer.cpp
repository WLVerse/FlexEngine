// WLVERSE [https://wlverse.web.app]
// physicslayer.cpp
// 
// Physics layer for the editor.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

#include <Physics/physicssystem.h>

namespace Game
{

  void PhysicsLayer::OnAttach()
  {
  }

  void PhysicsLayer::OnDetach()
  {
  }

  void PhysicsLayer::Update()
  {
    FlexEngine::PhysicsSystem::UpdatePhysicsSystem();
  }

}
