#pragma once
#include <FlexEngine.h>
#include "imgui.h"

using namespace FlexEngine;

namespace ChronoShift {
  class PrefabEditor
  {
    static bool paused;

  public:
    PrefabEditor() = default;
    ~PrefabEditor() = default;

    // Create prefab window with IMGUI
    void ShowPrefabEditorWindow();
  };
}
