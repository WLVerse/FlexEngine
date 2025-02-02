#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void GameLayer::OnAttach()
  {
    #if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      FlexECS::Scene::SetActiveScene(scene);

      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 0");
        entity.AddComponent<Vector2>({ 2, 4 });
        entity.AddComponent<Transform>({});
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 1");
        entity.AddComponent<Vector2>({ 35, 42 });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({ true, Matrix4x4::Identity, true });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 2222222222222222222222222222222222");
        entity.AddComponent<Position>({ Vector3{0,0,0} });
        entity.AddComponent<Rotation>({ Vector3{0,0,0} });
        entity.AddComponent<Scale>({ {100,100,100} });
        entity.AddComponent<Transform>({});
        //entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CameraHandler)") });
        entity.AddComponent<Audio>({ true, false, false, false, FLX_STRING_NEW(R"(/audio/attack.mp3)") });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)"), -1 });
        entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Prop_Flaming_Barrel.flxspritesheet)"), true, 0.f });
        entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
      }
      //Camera Test
      {
        FlexECS::Entity cam = scene->CreateEntity("Test Cam");
        cam.AddComponent<Position>({});
        cam.AddComponent<Rotation>({});
        cam.AddComponent<Scale>({});
        cam.AddComponent<Transform>({});
        //There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write each exact variable
        //cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        // Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera({ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam.AddComponent<Camera>(gameTestCamera);

        FlexECS::Entity cam2 = scene->CreateEntity("Test Cam2");
        cam2.AddComponent<Position>({});
        cam2.AddComponent<Rotation>({});
        cam2.AddComponent<Scale>({});
        cam2.AddComponent<Transform>({});
        //There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write each exact variable
        //cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        // Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera2({ 650.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam2.AddComponent<Camera>(gameTestCamera2);
      }
      //Text test
      {
        FlexECS::Entity txt = scene->CreateEntity("Test Text");
        txt.AddComponent<Position>({ Vector3(822.0f, 248.0f, 0.0f) });
        txt.AddComponent<Rotation>({});
        txt.AddComponent<Scale>({});
        txt.AddComponent<Transform>({});
        txt.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Bangers/Bangers-Regular.ttf)"),
                                 FLX_STRING_NEW(R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class))"),
                                 Vector3(1.0f,0.0,0.0f),
                                 {Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle} });
        //button
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
          {
            Log::Info("Found " + FLX_STRING_GET(*entity.GetComponent<EntityName>()));
          }
          else
          {
            Log::Error("Entity not found");
          }
        }
        //scene->DumpArchetypeIndex();
      }
      #endif
    }
  }

  void GameLayer::OnDetach()
  {
  }

  void GameLayer::Update()
  {
  
  }
}
