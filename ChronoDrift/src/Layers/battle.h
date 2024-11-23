/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battle.h
\author    [100%] Rocky Sutarius
\par       rocky.sutarius\@digipen.edu
\date      03 October 2024
\brief     This header file is where the declarations of the
           functions to set up and run the battle scene of the
           game, is contained at.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <FlexEngine.h>

#include "BattleSystems/battlesystem.h"
#include "CharacterMoves/moves.h"
using namespace FlexEngine;

namespace ChronoDrift
{
  class BattleLayer : public FlexEngine::Layer
  {
  public:
    BattleLayer() : Layer("Battle Layer") {}
    ~BattleLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void SetupBattle();

    BattleSystem m_battlesystem;
  };
}
