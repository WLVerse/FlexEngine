// WLVERSE [https://wlverse.web.app]
// istate.h
// 
// The interface for states in the state manager.
// 
// Usage:
// 
// class YourState : public State
// {
// public:
//   void OnEnter() override
//   {
//     // your code here
//   }
// 
//   void Update() override
//   {
//     // your code here
//   }
// 
//   void OnExit() override
//   {
//     // your code here
//   }
// };
// 
// The state manager implementation itself uses the Template Method Pattern.
// It is a static class that doesn't need explicit instantiation.
// Create states using the State class and set them using the SetState method.
// 
// Usage: ApplicationStateManager::SetState<YourState>();
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
