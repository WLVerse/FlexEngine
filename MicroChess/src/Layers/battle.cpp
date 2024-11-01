/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battle.cpp
\author    [78%] Rocky Sutarius
\par       rocky.sutarius\@digipen.edu
\co-author [22%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file is where the functions to set up and run
           the battle scene of the game, is contained at. The
           functions include:
           - SetupBattle()
           - OnAttach()
           - OnDetach()
           - Update()

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "battle.h"

#include "States.h"
#include "Layers.h"

#include "Components/battlecomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"

#include "BattleSystems/turnorderdisplay.h"
#include "BattleSystems/targetingsystem.h"
#include "BattleSystems/battlesystem.h"

#include "Physics/box2d.h"
#include "Renderer/sprite2d.h"
#include "CharacterPrefab/characterprefab.h"


namespace ChronoShift {

  void BattleLayer::SetupBattle()
  {

    File& file = File::Open(Path::current().append("pee.flxscene"));
    auto loaded_scene = FlexECS::Scene::Load(file);
    FlexECS::Scene::SetActiveScene(loaded_scene);

    MoveRegistry::RegisterMoves();
    m_battlesystem.AddCharacters(FlexECS::Scene::GetActiveScene()->View<Character>());
    m_battlesystem.BeginBattle();

    /*************************************************************************
    Waiting on Rocky to remove this once he has created a sprite to render on imgui
     - Collision is off due to editor render and final render is on a sprite
     - This shouldnt be here, i just add this here as a quick fix because you wont see anything if so
    ************************************************************************/
    //auto scene = FlexECS::Scene::GetActiveScene();
    //FlexECS::Entity editorRender = FlexECS::Scene::CreateEntity("editorRender");
    //editorRender.AddComponent<IsActive>({ true });
    //editorRender.AddComponent<Position>({ {600, 300 } });
    //editorRender.AddComponent<Scale>({ { 800,800} });
    //editorRender.AddComponent<Rotation>({ });
    //editorRender.AddComponent<Transform>({});
    //editorRender.AddComponent<ZIndex>({ 9 });
    //editorRender.AddComponent<Sprite>({
    //    scene->Internal_StringStorage_New(R"()"),
    //    { 0.0f,0.0f, 0.0f },
    //    Vector3::Zero,
    //    Vector3::One,
    //    Renderer2DProps::Alignment_Center,
    //    Renderer2DProps::VBO_BasicInverted,
    //    false
    //   });
    //editorRender.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

    //FlexECS::Entity finalRender = FlexECS::Scene::CreateEntity("finalRender");
    //finalRender.AddComponent<IsActive>({ true });
    //finalRender.AddComponent<Position>({ {300, 600 } });
    //finalRender.AddComponent<Scale>({ { 300,300} });
    //finalRender.AddComponent<Rotation>({ });
    //finalRender.AddComponent<Transform>({});
    //finalRender.AddComponent<ZIndex>({ 10 });
    //finalRender.AddComponent<Sprite>({
    //    scene->Internal_StringStorage_New(R"()"),
    //    { 0.0f,0.0f, 0.0f },
    //    Vector3::Zero,
    //    Vector3::One,
    //    Renderer2DProps::Alignment_Center,
    //    Renderer2DProps::VBO_BasicInverted,
    //    false
    //   });
    //finalRender.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
  }

  void BattleLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // ECS Setup
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

    // Setup the board
    SetupBattle();
  }

  void BattleLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void BattleLayer::Update()
  {
    m_battlesystem.Update();

    DisplayTurnOrder(m_battlesystem.GetTurnOrder());
    RendererSprite2D();
    
    if (Input::GetKeyDown(GLFW_KEY_S)) {
      SaveCharacters();
    }
    else if (Input::GetKeyDown(GLFW_KEY_R)) {
      ResetCharacters();
    }
  }
}