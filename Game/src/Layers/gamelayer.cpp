#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void GameLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

    FlexECS::Entity entity = scene->CreateEntity("Grace");
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

    FlexECS::Entity cam = scene->CreateEntity("Cam");
    cam.AddComponent<Position>({});
    cam.AddComponent<Rotation>({});
    cam.AddComponent<Scale>({});
    cam.AddComponent<Transform>({});
    Camera gameTestCamera({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
    cam.AddComponent<Camera>(gameTestCamera);

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
  }

  void GameLayer::OnDetach()
  {
    // unfortunately fmod does not stop itself because a command is not sent to stop it on scene switch, so this is it
    FMODWrapper::Core::ForceStop();
  }

  void GameLayer::Update()
  {
    #pragma region Copied Camera Code
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
                entity.GetComponent<Camera>()->m_data.position.y -= speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();  // Move forward
            if (Input::GetKey('S'))
                entity.GetComponent<Camera>()->m_data.position.y += speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();  // Move backward
            if (Input::GetKey('A'))
                entity.GetComponent<Camera>()->m_data.position.x -= speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();  // Move left
            if (Input::GetKey('D'))
                entity.GetComponent<Camera>()->m_data.position.x += speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();  // Move right
            
            entity.GetComponent<Transform>()->is_dirty = true;
        }
    }

    #pragma endregion
  }
}
