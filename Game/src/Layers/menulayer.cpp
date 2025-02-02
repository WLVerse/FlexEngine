#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void MenuLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

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
