// WLVERSE [https://wlverse.web.app]
// state.h
// 
// Interface for states used by the state manager.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

namespace FlexEngine
{

  // abstract class for states
  class State
  {
  public:
    virtual ~State() = default;
    virtual void OnEnter() = 0;
    virtual void Update() = 0;
    virtual void OnExit() = 0;
  };

}
