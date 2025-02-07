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

namespace Editor
{

  void EditorBaseLayer::OnAttach()
  {

    #pragma region Create the entities
    #if 1
     {
      auto scene = FlexECS::Scene::CreateScene();
      /*{
        FlexECS::Entity entity = scene->CreateEntity("Save Test 00000000000000000000000000000000000000000");
        entity.AddComponent<Transform>({});
        entity.AddComponent<Sprite>({ FLX_STRING_NEW("") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Sprite2 Test");
        entity.AddComponent<Position>({ {600, 600, 0} });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Scale>({ {250, 250, 0} });
        entity.AddComponent<Transform>({});
        entity.AddComponent<Sprite>({ FLX_STRING_NEW("") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Sprite Test");
        entity.AddComponent<Position>({
          Vector3{ 100, 300, 0 }
        });
        entity.AddComponent<Rotation>({
          Vector3{ 0, 0, 35 }
        });
        entity.AddComponent<Scale>({
          { 100, 100, 100 }
        });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)") });
        entity.AddComponent<Transform>({ true, Matrix4x4::Identity, true });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Animator Sprite Test");
        entity.AddComponent<Position>({
          Vector3{ 100, 100, 0 }
        });
        entity.AddComponent<Rotation>({
          Vector3{ 0, 0, 0 }
        });
        entity.AddComponent<Scale>({
          { 100, 100, 100 }
        });
        entity.AddComponent<Transform>({});
        // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CameraHandler)") });
        entity.AddComponent<Audio>({ true, false, false, false, FLX_STRING_NEW(R"(/audio/attack.mp3)") });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)") });
        entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)"), true, 0.f });
        entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      // Camera Test
      {
        FlexECS::Entity cam = scene->CreateEntity("Test Cam");
        cam.AddComponent<Position>({});
        cam.AddComponent<Rotation>({});
        cam.AddComponent<Scale>({});
        cam.AddComponent<Transform>({});
        // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
        // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        //  Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam.AddComponent<Camera>(gameTestCamera);

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
      }
      // Text test
      {
        FlexECS::Entity txt = scene->CreateEntity("Test Text");
        txt.AddComponent<Position>({ Vector3(822.0f, 248.0f, 0.0f) });
        txt.AddComponent<Rotation>({});
        txt.AddComponent<Scale>({});
        txt.AddComponent<Transform>({});
        txt.AddComponent<ZIndex>({ 1000 });
        txt.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Bangers/Bangers-Regular.ttf)"),
          FLX_STRING_NEW(
            R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class))"
          ),
          Vector3(1.0f, 0.0, 0.0f),
          { Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Middle }
        });
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
      */
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
          entity.AddComponent<Character>({});
          entity.GetComponent<Character>()->character_name = FLX_STRING_NEW("Renko");
          entity.GetComponent<Character>()->base_speed = 3;
          
          entity.GetComponent<Character>()->skill_one.damage_duration = 5;
          entity.GetComponent<Character>()->skill_one.speed = 3;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("Revolution");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Deal 5 damage to all enemies. Costs 3 Move Speed.");
          entity.GetComponent<Character>()->skill_one.effect_name = FLX_STRING_NEW("DAMAGE");
          entity.GetComponent<Character>()->skill_one.target = 6;

          entity.GetComponent<Character>()->skill_two.damage_duration = 6;
          entity.GetComponent<Character>()->skill_two.speed = 1;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("Strike");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Deal 6 damage to an enemy. Costs 1 Move Speed.");
          entity.GetComponent<Character>()->skill_two.effect_name = FLX_STRING_NEW("DAMAGE");
          entity.GetComponent<Character>()->skill_two.target = 4;

          entity.GetComponent<Character>()->skill_three.damage_duration = 10;
          entity.GetComponent<Character>()->skill_three.speed = 9;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("Hour of Reckoning");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Deal 10 damage to all enemies. Costs 9 Move Speed.");
          entity.GetComponent<Character>()->skill_three.effect_name = FLX_STRING_NEW("DAMAGE");
          entity.GetComponent<Character>()->skill_three.target = 6;
          /*entity.GetComponent<Character>()->skill_one.damage = 5;
          entity.GetComponent<Character>()->skill_one.speed = 3;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("Revolution");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Deal 5 damage to all enemies. Costs 3 Move Speed.");
          //entity.GetComponent<Character>()->skill_one.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_one.damage_duration.push_back(5);
          //entity.GetComponent<Character>()->skill_one.target.push_back(6);

          entity.GetComponent<Character>()->skill_two.damage = 6;
          entity.GetComponent<Character>()->skill_two.speed = 1;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("Strike");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Deal 6 damage to an enemy. Costs 1 Move Speed.");
          //entity.GetComponent<Character>()->skill_two.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_two.damage_duration.push_back(6);
          //entity.GetComponent<Character>()->skill_two.target.push_back(4);

          entity.GetComponent<Character>()->skill_three.damage = 10;
          entity.GetComponent<Character>()->skill_three.speed = 9;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("Hour of Reckoning");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Deal 10 damage to all enemies. Costs 9 Move Speed.");
          //entity.GetComponent<Character>()->skill_three.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_three.damage_duration.push_back(10);
          //entity.GetComponent<Character>()->skill_three.target.push_back(6);

          /*Effect current_effect{FLX_STRING_NEW("DAMAGE"), 5, 6};
          entity.GetComponent<Character>()->skill_one.damage = 5;
          entity.GetComponent<Character>()->skill_one.speed = 3;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("Revolution");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Deal 5 damage to all enemies. Costs 3 Move Speed.");
          entity.GetComponent<Character>()->skill_one.effect.push_back(current_effect);
          current_effect = { FLX_STRING_NEW("DAMAGE"), 6, 4 };
          entity.GetComponent<Character>()->skill_two.damage = 6;
          entity.GetComponent<Character>()->skill_two.speed = 1;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("Strike");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Deal 6 damage to an enemy. Costs 1 Move Speed.");
          entity.GetComponent<Character>()->skill_two.effect.push_back(current_effect);
          current_effect = { FLX_STRING_NEW("DAMAGE"), 10, 6 };
          entity.GetComponent<Character>()->skill_three.damage = 10;
          entity.GetComponent<Character>()->skill_three.speed = 9;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("Hour of Reckoning");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Deal 10 damage to all enemies. Costs 9 Move Speed.");
          entity.GetComponent<Character>()->skill_three.effect.push_back(current_effect);*/
          //  move_num.effect.push_back(current_effect);
          //entity.GetComponent<Character>()->base_speed = 1;
          //entity.GetComponent<Character>()->base_speed = 1;
          //entity.GetComponent<Character>()->base_speed = 1;

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
          entity.AddComponent<Character>({});
          entity.GetComponent<Character>()->character_name = FLX_STRING_NEW("Grace");
          entity.GetComponent<Character>()->base_speed = 1;

          entity.GetComponent<Character>()->skill_one.damage_duration = 1;
          entity.GetComponent<Character>()->skill_one.speed = 5;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("Discipline");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Increase the next fastest ally's ATK for 1 turn. Costs 5 Move Speed.");
          entity.GetComponent<Character>()->skill_one.effect_name = FLX_STRING_NEW("ATTACK_BUFF");
          entity.GetComponent<Character>()->skill_one.target = 1;

          entity.GetComponent<Character>()->skill_two.damage_duration = 0;
          entity.GetComponent<Character>()->skill_two.speed = 6;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("Timeout");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Decrease an enemy's attack by 2 turns. Costs 6 Move Speed.");
          entity.GetComponent<Character>()->skill_two.effect_name = FLX_STRING_NEW("ATTACK_DEBUFF");
          entity.GetComponent<Character>()->skill_two.target = 4;

          entity.GetComponent<Character>()->skill_three.damage_duration = 0;
          entity.GetComponent<Character>()->skill_three.speed = 11;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("Innocent Verdict");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Makes all allies immune to damage for 1 turn. Costs 11 Move Speed.");
          entity.GetComponent<Character>()->skill_three.effect_name = FLX_STRING_NEW("INVULN_BUFF");
          entity.GetComponent<Character>()->skill_three.target = 3;
          
          /*entity.GetComponent<Character>()->skill_one.damage = 5;
          entity.GetComponent<Character>()->skill_one.speed = 5;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("Discipline");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Deal 5 damage to an enemy. Increase the next fastest ally's ATK for 1 turn. Costs 5 Move Speed.");
          //entity.GetComponent<Character>()->skill_one.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_one.damage_duration.push_back(5);
          //entity.GetComponent<Character>()->skill_one.target.push_back(4);

          entity.GetComponent<Character>()->skill_one.effect_name.push_back(FLX_STRING_NEW("ATTACK_BUFF"));
          entity.GetComponent<Character>()->skill_one.damage_duration.push_back(1);
          entity.GetComponent<Character>()->skill_one.target.push_back(2);

          entity.GetComponent<Character>()->skill_two.damage = 0;
          entity.GetComponent<Character>()->skill_two.speed = 6;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("Timeout");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Decrease an enemy's attack by 2 turns. Costs 6 Move Speed.");
          //entity.GetComponent<Character>()->skill_two.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_two.damage_duration.push_back(0);
          //entity.GetComponent<Character>()->skill_two.target.push_back(4);

          entity.GetComponent<Character>()->skill_two.effect_name.push_back(FLX_STRING_NEW("ATTACK_DEBUFF"));
          entity.GetComponent<Character>()->skill_two.damage_duration.push_back(2);
          entity.GetComponent<Character>()->skill_two.target.push_back(4);

          entity.GetComponent<Character>()->skill_three.damage = 0;
          entity.GetComponent<Character>()->skill_three.speed = 11;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("Innocent Verdict");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Makes all allies immune to damage for 1 turn. Costs 11 Move Speed.");
          //entity.GetComponent<Character>()->skill_three.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_three.damage_duration.push_back(0);
          //entity.GetComponent<Character>()->skill_three.target.push_back(3);

          //entity.GetComponent<Character>()->skill_three.effect_name.push_back(FLX_STRING_NEW("INVULN_BUFF"));
          //entity.GetComponent<Character>()->skill_three.damage_duration.push_back(1);
          //entity.GetComponent<Character>()->skill_three.target.push_back(3);*/
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
          entity.AddComponent<Character>({});
          entity.GetComponent<Character>()->character_name = FLX_STRING_NEW("Jack");
          entity.GetComponent<Character>()->base_speed = 2;
          entity.GetComponent<Character>()->base_hp = 35;

          entity.GetComponent<Character>()->skill_one.damage_duration = 5;
          entity.GetComponent<Character>()->skill_one.speed = 6;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("An Inopportune Greeting");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Strip all enemy buffs and deal 5 damage to all enemies. Costs 6 Move Speed.");
          entity.GetComponent<Character>()->skill_one.effect_name = FLX_STRING_NEW("DAMAGE");
          entity.GetComponent<Character>()->skill_one.target = 3;

          entity.GetComponent<Character>()->skill_two.damage_duration = 8;
          entity.GetComponent<Character>()->skill_two.speed = 3;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("A Theatrical Farce");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Deal 8 damage to all enemies. Costs 3 Move Speed.");
          entity.GetComponent<Character>()->skill_two.effect_name = FLX_STRING_NEW("DAMAGE");
          entity.GetComponent<Character>()->skill_two.target = 3;

          entity.GetComponent<Character>()->skill_three.damage_duration = 20;
          entity.GetComponent<Character>()->skill_three.speed = 2;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("End of Time");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Deal 20 damage to all enemies. Costs 2 Move Speed.");
          entity.GetComponent<Character>()->skill_three.effect_name = FLX_STRING_NEW("DAMAGE");
          entity.GetComponent<Character>()->skill_three.target = 3;

          /*entity.GetComponent<Character>()->skill_one.damage = 5;
          entity.GetComponent<Character>()->skill_one.speed = 6;
          entity.GetComponent<Character>()->skill_one.name = FLX_STRING_NEW("An Inopportune Greeting");
          entity.GetComponent<Character>()->skill_one.description = FLX_STRING_NEW("Strip all enemy buffs and deal 5 damage to all enemies. Costs 6 Move Speed.");
          //entity.GetComponent<Character>()->skill_one.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_one.damage_duration.push_back(5);
          //entity.GetComponent<Character>()->skill_one.target.push_back(6);

          entity.GetComponent<Character>()->skill_two.damage = 8;
          entity.GetComponent<Character>()->skill_two.speed = 3;
          entity.GetComponent<Character>()->skill_two.name = FLX_STRING_NEW("A Theatrical Farce");
          entity.GetComponent<Character>()->skill_two.description = FLX_STRING_NEW("Deal 8 damage to all enemies. Costs 3 Move Speed.");
          //entity.GetComponent<Character>()->skill_two.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_two.damage_duration.push_back(8);
          //entity.GetComponent<Character>()->skill_two.target.push_back(6);

          entity.GetComponent<Character>()->skill_three.damage = 20;
          entity.GetComponent<Character>()->skill_three.speed = 2;
          entity.GetComponent<Character>()->skill_three.name = FLX_STRING_NEW("End of Time");
          entity.GetComponent<Character>()->skill_three.description = FLX_STRING_NEW("Deal 20 damage to all enemies. Costs 2 Move Speed.");
          //entity.GetComponent<Character>()->skill_three.effect_name.push_back(FLX_STRING_NEW("DAMAGE"));
          //entity.GetComponent<Character>()->skill_three.damage_duration.push_back(20);
          //entity.GetComponent<Character>()->skill_three.target.push_back(6);*/
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
      //Particle System test
      {
        //FlexECS::Entity emitter = scene->CreateEntity("Test Spawner");
        //emitter.AddComponent<Position>({ Vector3(200.0f, -200.0f, 0.0f) });
        //emitter.AddComponent<Rotation>({Vector3(0,0,180)}); //In radians ah
        //emitter.AddComponent<Scale>({ Vector3(1.0f, 1.0f, 0.0f) });
        //emitter.AddComponent<Transform>({});
        //emitter.AddComponent<Sprite>({});
        //emitter.AddComponent<ParticleSystem>({});
        ////For particle systems no-choice easier to do this if manual -> too many variables
        //emitter.GetComponent<ParticleSystem>()->particlesprite_handle = FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)");
        //emitter.GetComponent<ParticleSystem>()->particleEmissionShapeIndex = (int)ParticleEmitShape::Hemisphere;

        //FlexECS::Entity emitter2 = scene->CreateEntity("Test Spawner 2");
        //emitter2.AddComponent<Position>({ Vector3(600.0f, -200.0f, 0.0f) });
        //emitter2.AddComponent<Rotation>({ Vector3(0.0f,0.0f,90.0f)});
        //emitter2.AddComponent<Scale>({ Vector3(1.0f, 1.0f, 0.0f) });
        //emitter2.AddComponent<Transform>({});
        //emitter2.AddComponent<Sprite>({});
        //emitter2.AddComponent<ParticleSystem>({});
        //emitter2.GetComponent<ParticleSystem>()->particleEmissionShapeIndex = (int)ParticleEmitShape::Box;

      }
      //scene->DumpArchetypeIndex();
      // test find
      {
        scene->CreateEntity("Waldo!");
        auto entity = scene->GetEntityByName("Waldo!");
        if (entity)
          Log::Info("Found " + FLX_STRING_GET(*entity.GetComponent<EntityName>()));
        else
          Log::Error("Entity not found");
      }
      // scene->DumpArchetypeIndex();
    }
    #endif


    #pragma endregion

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

    OpenGLFrameBuffer::Unbind();

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
            FlexECS::Scene::SetActiveScene(FlexECS::Scene::CreateScene());
            current_save_name = default_save_name;
          } });
        }

        if (ImGui::MenuItem("Open", "Ctrl+O"))
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
                                    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
                                  } });
        }

        if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
        {
          function_queue.Insert({ [this]()
                                  {
                                    FileList files = FileList::Browse(
                                      "Save Scene", current_save_directory, current_save_name + ".flxscene",
                                      L"FlexScene Files (*.flxscene)\0"
                                      L"*.flxscene\0"
                                      L"All Files (*.*)\0"
                                      L"*.*\0",
                                      true
                                    );

                                    // cancel if no files were selected
                                    if (files.empty()) return;

                                    // sanitize the file extension
                                    std::string file_path = files[0];
                                    auto it = file_path.find_last_of(".");
                                    if (it != std::string::npos) file_path = file_path.substr(0, it);
                                    file_path += ".flxscene";

                                    // open the file
                                    File& file = File::Open(Path(file_path));

                                    // save the scene
                                    FlexECS::Scene::SaveActiveScene(file);
                                    Log::Info("Saved scene to: " + file.path.string());

                                    // update the current save directory and name
                                    current_save_directory = file.path.parent_path();
                                    current_save_name = file.path.stem().string();
                                  } });
        }

        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    } // namespace Editor
  }
}