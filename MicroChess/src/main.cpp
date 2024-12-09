#include <FlexEngine.h>
#include <FlexEngine/Core/EntryPoint.h>

#include "States.h"

namespace MicroChess
{
  class FlexApplication : public FlexEngine::Application
  {
  public:
    FlexApplication()
    {
      FlexEngine::ApplicationStateManager::SetState<MainState>();
    }
  };

}

namespace FlexEngine
{
  Application* CreateApplication()
  {
    return new MicroChess::FlexApplication();
  }
}
