/*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      mainstate.cpp
\author    [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
\date      27/11/2024
\brief     Implementation of main state


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "mainstate.h"

#include "States.h"
#include "Layers.h"

namespace ChronoDrift
{

  void MainState::OnEnter()
  {
    window = Application::OpenWindow(
      {
        "Chrono Drift - FlexEngine",
        1280, 720 + 30, // 30 pixels for the title bar
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, false },
          { GLFW_RESIZABLE, false }
        }
      }
    );
    #ifndef GAME
    AlignImGuiContext(window);
    #endif
    window->PushLayer(std::make_shared<BaseLayer>());
  }

  void MainState::OnExit()
  {
    Application::CloseWindow(window);
    window.reset();
  }

  void MainState::Update()
  {
    window->Update();
  }

}