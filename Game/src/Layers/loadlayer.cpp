#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void LoadLayer::OnAttach()
  {
    AssetManager::Load();
    FreeQueue::Push(std::bind(&AssetManager::Unload), "Application AssetManager");
  }

  void LoadLayer::OnDetach()
  {
  }

  void LoadLayer::Update()
  {
  }
}
