/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      prefabeditor.h
\author    [100%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      27 November 2024
\brief     This file is where the functions utilized by the
           prefab editor to edit prefabs, is declared at.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <FlexEngine.h>
#include "imgui.h"

using namespace FlexEngine;

namespace ChronoDrift {
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
