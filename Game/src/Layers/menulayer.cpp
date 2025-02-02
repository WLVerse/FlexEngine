#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void MenuLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);
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

    Log::Info("Menu Layer Attach");
  }

  void MenuLayer::OnDetach()
  {
    Log::Info("Menu Layer Detach");
  }

  void MenuLayer::Update()
  {
  }
}
