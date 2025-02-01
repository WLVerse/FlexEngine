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

#include "FlexECS/datastructures.h"

#include <string>

#pragma region Macros



#pragma endregion

namespace FlexEngine
{

  class IScript
  {
    #pragma region Script Context

    // The context that the script is running in.
    // This is the entity that the script is attached to,
    // and it is used to access the components of the entity.

  private:
    FlexECS::Entity context = FlexECS::Entity::Null;

  public:
    // INTERNAL FUNCTION
    // This is set by the scripting system, do not call this function manually.
    void Internal_SetContext(const FlexECS::Entity& id = FlexECS::Entity::Null) { context = id; }

    // INTERNAL FUNCTION
    // This is used to get the entity index of the script.
    FlexECS::Entity Internal_GetContext() const { return context; }

    #ifdef self
    #error "Conflicting macro name: self"
    #endif
    #define self Internal_GetContext()

    #pragma endregion

  public:
    virtual ~IScript() = default;

    virtual void Awake() {};
    virtual void Start() {};
    virtual void Update() {};
    //virtual void FixedUpdate() {};
    virtual void Stop() {};

    #pragma region UI Callbacks

    // TODO: replace with proper ui system
    virtual void OnHover() {};
    virtual void OnClick() {};

    #pragma endregion

    #pragma region Physics Callbacks

    //virtual void OnCollisionEnter() {};
    //virtual void OnCollisionExit() {};
    //virtual void OnCollisionStay() {};

    #pragma endregion

    virtual std::string GetName() const = 0;
  };

}
