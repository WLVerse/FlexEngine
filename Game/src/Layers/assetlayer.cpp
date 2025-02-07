#include "Layers.h"

namespace Game
{
  void AssetLayer::OnAttach()
  {
    AssetManager::Load();
    FreeQueue::Push(std::bind(&AssetManager::Unload), "Application AssetManager");
  }

  void AssetLayer::OnDetach()
  {
    FreeQueue::RemoveAndExecute("Application AssetManager");
  }

  void AssetLayer::Update()
  {
  }
} // namespace Game
