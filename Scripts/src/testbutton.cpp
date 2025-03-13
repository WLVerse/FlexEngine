/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// testbutton.cpp
//
// This script is used to test whenever mouse clicks on a button. 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
using namespace FlexEngine;

class TestButtonScript : public IScript
{
public:
  TestButtonScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "TestButton"; }

  void OnMouseEnter() override
  {
    Log::Debug("Mouse entered: " + FLX_STRING_GET(*self.GetComponent<EntityName>()));
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Log::Debug("Mouse clicked: " + FLX_STRING_GET(*self.GetComponent<EntityName>()));
    }
  }

  void OnMouseExit() override
  {
    Log::Debug("Mouse exited: " + FLX_STRING_GET(*self.GetComponent<EntityName>()));
  }
};

// Static instance to ensure registration
static TestButtonScript TestButton;