#include "scripting.h"

namespace ChronoShift {

  void ScriptingLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
    scriptingSystem.LoadDLL("Scripting.dll");
  }

  void ScriptingLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
    scriptingSystem.UnloadDLL();
  }

  void ScriptingLayer::Update()
  {

  }
}