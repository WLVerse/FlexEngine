#include "Layers.h"

namespace Game {
  void TownLayer::OnAttach() {
    #pragma region Create the entities
    #if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      {
        FlexECS::Entity entity = scene->CreateEntity("Town Image");
        entity.AddComponent<Position>({ { 750, 400, 0 } });
        entity.AddComponent<Scale>({ { 5000, 5000, 0 } });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/Env_Town_Build_M3_Blocking_04_WithOverlay.png)") });
      }
      {
        main_character = scene->CreateEntity("Walking Renko");
        main_character.AddComponent<Position>({ {750, 400, 0} });
        main_character.AddComponent<Scale>({ {80, 186, 0} });
        main_character.AddComponent<Rotation>({});
        main_character.AddComponent<Transform>({});
        main_character.AddComponent<ZIndex>({1});
        main_character.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_renko.png)") });
        main_character.AddComponent<Script>({ FLX_STRING_NEW("MovePlayer") });
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
      }
    }
    #endif
  }
  void TownLayer::OnDetach() {

  }
  void TownLayer::Update() {
    CameraManager::GetMainGameCamera()->m_data.position = main_character.GetComponent<Position>()->position;
  }
}