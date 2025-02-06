// WLVERSE [https://wlverse.web.app]
// editorbaselayer.h
//
// Base layer for the editor.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "FlexEngine/FlexECS/enginecomponents.h"
#include "Layers.h"
#include "componentviewer.h"
#include "editor.h"
#include "editorcomponents.h"
#include "../../Scripts/src/character.h"

namespace Editor
{

    void EditorBaseLayer::OnAttach()
    {
      auto scene = FlexECS::Scene::CreateScene();
        FlexECS::Entity cam2 = scene->CreateEntity("Test Cam2");
        cam2.AddComponent<Position>({});
        cam2.AddComponent<Rotation>({});
        cam2.AddComponent<Scale>({});
        cam2.AddComponent<Transform>({});
        // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
        // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        //  Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera2({ 650.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam2.AddComponent<Camera>(gameTestCamera2);
      
      // BG
      {
          FlexECS::Entity entity = scene->CreateEntity("Background");
          entity.AddComponent<Position>({
            Vector3{ -1450, -1000, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 3104, 2432, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/Env_Town_Build_M3_Blocking_04_WithOverlay.png)"), -1 });
          // 
          //entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/Env_Town_Build_M4_01.png)"), -1 });
          // 
           //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Grace_Idle_Attack_Anim_Sheet.flxspritesheet)"),
         // true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // BG
      /* {
          FlexECS::Entity entity = scene->CreateEntity("BG");
          entity.AddComponent<Position>({
            Vector3{ 850,450,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 1920, 1080, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/Background_Final.jpg)"), -1 });
      }*/
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Name");
          entity.AddComponent<Position>({
            Vector3{ 636, 525, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.15f, 0.15f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Renko)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko S1 Button");
          entity.AddComponent<Position>({
            Vector3{ 660, 420, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 81, 26, 1 }
          });
          entity.AddComponent<Transform>({});

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuButton_MouseOn.png)"), -1 });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko S1 Text");
          entity.AddComponent<Position>({
            Vector3{ 700, 435, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.1f, 0.1f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Move 1)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko S2 Button");
          entity.AddComponent<Position>({
            Vector3{ 660, 450, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 81, 26, 1 }
          });
          entity.AddComponent<Transform>({});

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuButton_MouseOn.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko S2 Text");
          entity.AddComponent<Position>({
            Vector3{ 700, 465, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.1f, 0.1f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Move 2)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko S3 Button");
          entity.AddComponent<Position>({
            Vector3{ 660, 480, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 81, 26, 1 }
          });
          entity.AddComponent<Transform>({});

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuButton_MouseOn.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko S3 Text");
          entity.AddComponent<Position>({
            Vector3{ 700, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.1f, 0.1f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Move 3)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko HP Bar");
          entity.AddComponent<Position>({
            Vector3{ 625, 525, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 35, 2, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_HealthBar_Green.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko HP Text");
          entity.AddComponent<Position>({
            Vector3{ 645, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.05f, 0.05f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(HP: 10/10 | bSPD: 5)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Attack Buff");
          entity.AddComponent<Position>({
            Vector3{ 615, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Attack_+1.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Attack Debuff");
          entity.AddComponent<Position>({
            Vector3{ 625, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Attack_-1.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Invuln Buff");
          entity.AddComponent<Position>({
            Vector3{ 635, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Def.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Stun Debuff");
          entity.AddComponent<Position>({
            Vector3{ 645, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Heal_-2.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Name");
          entity.AddComponent<Position>({
            Vector3{ 775, 490, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.15f, 0.15f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                                   FLX_STRING_NEW(R"(Grace)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace S1 Button");
          entity.AddComponent<Position>({
            Vector3{ 810, 390, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 81, 26, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuButton_MouseOn.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace S1 Text");
          entity.AddComponent<Position>({
            Vector3{ 850, 405, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.1f, 0.1f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Move 1)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace S2 Button");
          entity.AddComponent<Position>({
            Vector3{ 810, 420, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 81, 26, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuButton_MouseOn.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace S2 Text");
          entity.AddComponent<Position>({
            Vector3{ 850, 435, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.1f, 0.1f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Move 2)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace S3 Button");
          entity.AddComponent<Position>({
            Vector3{ 810, 450, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 81, 26, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuButton_MouseOn.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace S3 Text");
          entity.AddComponent<Position>({
            Vector3{ 850, 465, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.1f, 0.1f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Move 3)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace HP Bar");
          entity.AddComponent<Position>({
            Vector3{ 765, 490, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 35, 2, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_HealthBar_Green.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace HP Text");
          entity.AddComponent<Position>({
            Vector3{ 785, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.05f, 0.05f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(HP: 10/10 | bSPD: 5)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Attack Buff");
          entity.AddComponent<Position>({
            Vector3{ 755, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Attack_+1.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Attack Debuff");
          entity.AddComponent<Position>({
            Vector3{ 765, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Attack_-1.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Invuln Buff");
          entity.AddComponent<Position>({
            Vector3{ 775, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Def.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Stun Debuff");
          entity.AddComponent<Position>({
            Vector3{ 785, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Heal_-2.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Skill Border");
          entity.AddComponent<Position>({
            Vector3{ 745, 450, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 129, 69, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuBox.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Renko UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Skill Text");
          entity.AddComponent<Position>({
            Vector3{ 810, 485, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.15f, 0.15f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Skill Border");
          entity.AddComponent<Position>({
            Vector3{ 895, 420, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 129, 69, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuBox.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Skill Text");
          entity.AddComponent<Position>({
            Vector3{ 960, 455, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.15f, 0.15f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }

      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Name");
          entity.AddComponent<Position>({
            Vector3{ 1145, 525, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.15f, 0.15f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                                   FLX_STRING_NEW(R"(Jack The Ripper)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }

      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Skill Button");
          entity.AddComponent<Position>({
            Vector3{ 1115, 525, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 7, 7, 1 }
          });
          entity.AddComponent<Transform>({});

          entity.AddComponent<Button>({});
          entity.AddComponent<BoundingBox2D>({});
          entity.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Question Mark.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Skill Border");
          entity.AddComponent<Position>({
            Vector3{ 980, 495, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 129, 69, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_MenuBox.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Skill Text");
          entity.AddComponent<Position>({
            Vector3{ 1045, 535, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.15f, 0.15f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack HP Bar");
          entity.AddComponent<Position>({
            Vector3{ 1125, 525, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 35, 2, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_HealthBar_Green.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack HP Text");
          entity.AddComponent<Position>({
            Vector3{ 1145 , 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 0.05f, 0.05f, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(HP: 10/10 | bSPD: 5)"),
                                   Vector3(1.0f,1.0,1.0f),
                                   {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Attack Buff");
          entity.AddComponent<Position>({
            Vector3{ 1115, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Attack_+1.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Attack Debuff");
          entity.AddComponent<Position>({
            Vector3{ 1125, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Attack_-1.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Invuln Buff");
          entity.AddComponent<Position>({
            Vector3{ 1135, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Def.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Stun Debuff");
          entity.AddComponent<Position>({
            Vector3{ 1145, 530, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 24, 14, 1 }
          });
          entity.AddComponent<Transform>({});
          // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Heal_-2.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }

      {
          FlexECS::Entity entity = scene->CreateEntity("Renko");
          entity.AddComponent<Position>({
            Vector3{ 620, 420, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 40, 93, 1 }
          });
          entity.AddComponent<Transform>({});
          //entity.AddComponent<Script>({ FLX_STRING_NEW(R"(CharacterTest)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_renko.png)"), -1 });
          entity.AddComponent<Character>();
          entity.GetComponent<Character>()->character_name = "Renko";
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Grace
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace");
          entity.AddComponent<Position>({
            Vector3{ 740, 390, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 71, 90, 1 }
          });
          entity.AddComponent<Transform>({});
          //entity.AddComponent<Script>({ FLX_STRING_NEW(R"(CharacterTest)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Grace_Idle_Attack_Anim_Sheet.flxspritesheet)"),
        // true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Jack
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack");
          entity.AddComponent<Position>({
            Vector3{ 1100, 405, 0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 72, 108, 1 }
          });
          entity.AddComponent<Transform>({});
          //entity.AddComponent<Script>({ FLX_STRING_NEW(R"(CharacterInfo)") });
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_jack.png)"), -1 });
          //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
          //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      //speed bar UI
      {
          FlexECS::Entity entity = scene->CreateEntity("Speed Bar");
          entity.AddComponent<Position>({
            Vector3{ 420,450,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 270 }
          });
          entity.AddComponent<Scale>({
            { 187, 4, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI_BattleScreen_Speed Bar_02.png)"), -1 });
      }
      //speed bar Renko icon
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Portrait");
          entity.AddComponent<Position>({
            Vector3{ 483,470,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 60, 60, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Portrait_Big_Ally.png)"), -1 });
      }
      //speed bar Renko icon
      {
          FlexECS::Entity entity = scene->CreateEntity("Renko Icon");
          entity.AddComponent<Position>({
            Vector3{ 483,470,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 60, 60, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Portrait_Renko.png)"), -1 });
      }
      //speed bar Jack icon
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Portrait");
          entity.AddComponent<Position>({
            Vector3{ 483,400,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 60, 60, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Portrait_Big_Enemy.png)"), -1 });
      }
      //speed bar Jack icon
      {
          FlexECS::Entity entity = scene->CreateEntity("Jack Icon");
          entity.AddComponent<Position>({
            Vector3{ 483,400,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 60, 60, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Portrait_Jack.png)"), -1 });
      }
      //speed bar Grace icon
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Portrait");
          entity.AddComponent<Position>({
            Vector3{ 483,330,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 60, 60, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Portrait_Big_Ally.png)"), -1 });
      }
      //speed bar Grace icon
      {
          FlexECS::Entity entity = scene->CreateEntity("Grace Icon");
          entity.AddComponent<Position>({
            Vector3{ 483,330,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 60, 60, 1 }
          });
          entity.AddComponent<Transform>({});
          entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_BattleScreen_Portrait_Grace.png)"), -1 });
      }
      //win screen
      {
          FlexECS::Entity entity = scene->CreateEntity("Win Screen");
          entity.AddComponent<Position>({
            Vector3{ 850,450,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 1920, 1080, 1 }
          });
          entity.AddComponent<Transform>({});
          //entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_Win_Mock_Up.png)"), -1 });
      }
      //win screen
      {
          FlexECS::Entity entity = scene->CreateEntity("Lose Screen");
          entity.AddComponent<Position>({
            Vector3{ 850,450,0 }
          });
          entity.AddComponent<Rotation>({
            Vector3{ 0, -180, 180 }
          });
          entity.AddComponent<Scale>({
            { 1920, 1080, 1 }
          });
          entity.AddComponent<Transform>({});
          //entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/UI/UI_Lose_Mock_Up.png)"), -1 });
      }

      // Text test
      {
        FlexECS::Entity txt = scene->CreateEntity("Test Text");
        txt.AddComponent<Position>({ Vector3(822.0f, 248.0f, 0.0f) });
        txt.AddComponent<Rotation>({});
        txt.AddComponent<Scale>({});
        txt.AddComponent<Transform>({});
        txt.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Bangers/Bangers-Regular.ttf)"),
          FLX_STRING_NEW(
            R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class))"
          ),
          Vector3(1.0f, 0.0, 0.0f),
          { Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Middle }
        });

  #if 0 // Test case
          static std::string extra = "";
          if (Input::GetKey(GLFW_KEY_J))
              extra += "j";
          Renderer2DText sample;
          static std::string fullText = "The whole human fraternity is becoming highly dependent on the computer technology; no one can imagine life without computer. As, it has spread its wings so deeply in every area and made people used of it. It is very beneficial for the students of any class. They can use it to prepare their projects, learn poems, read different stories, download notes for exam preparations, collect large information within seconds, learn about painting, drawing, etc. However it enhances the professional skills of the students and helps in getting job easily.";
          static std::string displayedText = ""; // Start with an empty string
          static float elapsedTime = 0.0f;       // To track time
          elapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 100;
          if (elapsedTime >= 1.0f && displayedText.size() < fullText.size()) {
              displayedText += fullText[displayedText.size()]; // Append the next character
              elapsedTime = 0.0f; // Reset the timer
          }
          sample.m_words = displayedText;
          //sample.m_words = "hello there my name is markiplier and welcome back to another game of amnesia the dark descent" + extra;
          sample.m_color = Vector3::Zero;
          sample.m_fonttype = R"(/fonts/Bangers/Bangers-Regular.ttf)";
          sample.m_transform = Matrix4x4(1.00, 0.00, 0.00, 0.00,
              0.00, 1.00, 0.00, 0.00,
              0.00, 0.00, 1.00, 0.00,
               822.00, 248.00, 0.00, 1.00);
          sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
          //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Top };
          //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Middle };
          //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Bottom };
          //sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Top };
          sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle };
          //sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Bottom };
          //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Top };
          //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Middle };
          //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Bottom };
          sample.m_maxwidthtextbox = 850.0f;
  #endif
      }
      // button
      {
        FlexECS::Entity button = scene->CreateEntity("Test Button");
        button.AddComponent<Position>({ Vector3(200.0f, 200.0f, 0.0f) });
        button.AddComponent<Rotation>({});
        button.AddComponent<Scale>({ Vector3(300.0f, 100.0f, 0.0f) });
        button.AddComponent<Transform>({});
        button.AddComponent<Button>({});
        button.AddComponent<BoundingBox2D>({});
        button.AddComponent<Sprite>({});
        button.GetComponent<Sprite>()->center_aligned = true;
        button.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
      }
      // test find
      {
        scene->CreateEntity("Waldo!");
        auto entity = scene->GetEntityByName("Waldo!");
        if (entity)
          Log::Info("Found " + FLX_STRING_GET(*entity.GetComponent<EntityName>()));
        else
          Log::Error("Entity not found");
      }
    
        Editor::GetInstance().Init();
        RegisterComponents();


        // Add custom framebuffers
        Vector2 window_size = Vector2(static_cast<float>(Application::GetCurrentWindow()->GetWidth()), static_cast<float>(Application::GetCurrentWindow()->GetHeight()));
        Window::FrameBufferManager.AddFrameBuffer("Scene", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Game", window_size);

        // add physics layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<PhysicsLayer>());

        // add particle systems layer
        FLX_COMMAND_ADD_WINDOW_LAYER(
          "Editor",
          std::make_shared<ParticleSystemLayer>()
        );

        // add rendering layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<RenderingLayer>());

        // add audio layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<AudioLayer>());

        // add camera service layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<CameraSystemLayer>());
    }

    void EditorBaseLayer::OnDetach()
    {
    }

    void EditorBaseLayer::Update()
    {
        //Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
        //Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

        // Always remember to set the context before using ImGui
        FLX_IMGUI_ALIGNCONTEXT();

        // setup dockspace
        // ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode |
        // ImGuiDockNodeFlags_NoDockingInCentralNode;
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        // #pragma warning(suppress: 4189) // local variable is initialized but not referenced
        dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

        Editor::GetInstance().Update();

        // For IMGUI
        FunctionQueue function_queue;
        if (ImGui::BeginMainMenuBar())
        {
          if (ImGui::BeginMenu("File"))
          {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
              function_queue.Insert({ [this]()
                                      {
                                        // Clear the scene and reset statics
                                        Editor::GetInstance().GetSystem<SelectionSystem>()->ClearSelection();
                                        FlexECS::Scene::SetActiveScene(FlexECS::Scene::CreateScene());
                                        current_save_name = default_save_name;
                                      } });
            }

          // For IMGUI
          FunctionQueue function_queue;
          if (ImGui::BeginMainMenuBar())
          {
            function_queue.Insert({ [this]()
                                    {
                                      FileList files = FileList::Browse(
                                        "Open Scene", Path::current("saves"), "default.flxscene",
                                        L"FlexScene Files (*.flxscene)\0"
                                        L"*.flxscene\0",
                                        // L"All Files (*.*)\0" L"*.*\0",
                                        false, false
                                      );

                                      // cancel if no files were selected
                                      if (files.empty()) return;

                                      // open the scene
                                      File& file = File::Open(files[0]);

                                      // update the current save directory and name
                                      current_save_directory = file.path.parent_path();
                                      current_save_name = file.path.stem().string();

                                      // load the scene
                                      Editor::GetInstance().GetSystem<SelectionSystem>()->ClearSelection();
                                      FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
                                    } });
          }
          }
        }
        
        // Execute the function queue
        function_queue.Flush();
    }

} // namespace Editor




// preliminary test
#if 0
{
    for (auto& [type_list, archetype] : scene->archetype_index)
    {
        // test conversion
        FlexECS::_Archetype _archetype_index = FlexECS::Internal_ConvertToSerializedArchetype(archetype);

        // print the archetype
        std::stringstream ss;
        for (std::size_t i = 0; i < _archetype_index.type.size(); ++i)
        {
            ss << _archetype_index.type[i] << " ";
        }
        Log::Warning("Archetype: " + ss.str());

        // print what was converted
        for (std::size_t i = 0; i < _archetype_index.archetype_table.size(); ++i)
        {
            // print the type and data
            Log::Info("Type: " + _archetype_index.type[i]);

            // print per entity
            for (std::size_t j = 0; j < _archetype_index.archetype_table[i].size(); ++j)
            {
                Log::Debug("Entity: " + std::to_string(j));
                Log::Debug("Data: " + _archetype_index.archetype_table[i][j]);
            }
        }
    }
}
#endif

// test serialization
#if 0
{
    Path save_to_path = Path::current("temp/serialization_test_scene.flxscene");
    File& file = File::Open(save_to_path);
    scene->Save(file);
}
#endif

// test deserialization
#if 0
{
    Path load_from_path = Path::current("temp/serialization_test_scene.flxscene");
    File& file = File::Open(load_from_path);
    auto loaded_scene = FlexECS::Scene::Load(file);
    loaded_scene->DumpArchetypeIndex();
}
#endif


#if 0 // Test case
static std::string extra = "";
if (Input::GetKey(GLFW_KEY_J))
extra += "j";
Renderer2DText sample;
static std::string fullText = "The whole human fraternity is becoming highly dependent on the computer technology; no one can imagine life without computer. As, it has spread its wings so deeply in every area and made people used of it. It is very beneficial for the students of any class. They can use it to prepare their projects, learn poems, read different stories, download notes for exam preparations, collect large information within seconds, learn about painting, drawing, etc. However it enhances the professional skills of the students and helps in getting job easily.";
static std::string displayedText = ""; // Start with an empty string
static float elapsedTime = 0.0f;       // To track time
elapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 100;
if (elapsedTime >= 1.0f && displayedText.size() < fullText.size()) {
    displayedText += fullText[displayedText.size()]; // Append the next character
    elapsedTime = 0.0f; // Reset the timer
}
sample.m_words = displayedText;
//sample.m_words = "hello there my name is markiplier and welcome back to another game of amnesia the dark descent" + extra;
sample.m_color = Vector3::Zero;
sample.m_fonttype = R"(/fonts/Bangers/Bangers-Regular.ttf)";
sample.m_transform = Matrix4x4(1.00, 0.00, 0.00, 0.00,
    0.00, 1.00, 0.00, 0.00,
    0.00, 0.00, 1.00, 0.00,
     822.00, 248.00, 0.00, 1.00);
sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
//sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Top };
//sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Middle };
//sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Bottom };
//sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Top };
sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle };
//sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Bottom };
//sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Top };
//sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Middle };
//sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Bottom };
sample.m_maxwidthtextbox = 850.0f;
#endif


// Test for cam
#if 0
auto cameraEntities = FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>();

for (auto& entity : cameraEntities)
{

    auto camera = entity.GetComponent<Camera>();
    if (camera)
    {
        auto& position = camera->m_data.position;

        // Adjust movement speed as needed
        float speed = 300.0f;

        // Check for WASD input
        if (Input::GetKey('W')) // Replace 'W' with your input library's key codes
            entity.GetComponent<Camera>()->m_data.position.y -=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move forward
        if (Input::GetKey('S'))
            entity.GetComponent<Camera>()->m_data.position.y +=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move backward
        if (Input::GetKey('A'))
            entity.GetComponent<Camera>()->m_data.position.x -=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move left
        if (Input::GetKey('D'))
            entity.GetComponent<Camera>()->m_data.position.x +=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move right

        entity.GetComponent<Transform>()->is_dirty = true;
    }
}
#endif