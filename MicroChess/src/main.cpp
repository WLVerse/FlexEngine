#include <FlexEngine.h>
#include <entrypoint.h>

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
