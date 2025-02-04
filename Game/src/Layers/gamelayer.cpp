#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void GameLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);
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
