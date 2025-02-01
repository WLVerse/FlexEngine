#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void GameLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();

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

  void GameLayer::OnDetach()
  {
  }

  void GameLayer::Update()
  {
  }
}
