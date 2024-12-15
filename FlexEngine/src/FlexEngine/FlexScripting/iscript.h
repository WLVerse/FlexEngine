// WLVERSE [https://wlverse.web.app]
// iscript.h
// 
// The interface for scripts.
// Inherit from this class to create your own scripts,
// making sure they're only created in the scripting sln.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <string>

#pragma region Macros



#pragma endregion

namespace FlexEngine
{

  class Script
  {
  public:
    virtual ~Script() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    //virtual void FixedUpdate() = 0;
    virtual void Stop() = 0;
    virtual std::string GetName() const = 0;
  };

}
