/*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      mainstate.h
\author    [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
\date      27/11/2024
\brief     Main state layer declaration


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoDrift
{

  class MainState : public FlexEngine::State
  {
    std::shared_ptr<FlexEngine::Window> window;

  public:
    MainState() = default;
    ~MainState() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;
  };

}