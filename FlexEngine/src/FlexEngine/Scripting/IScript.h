#include "FlexECS/datastructures.h"

class IScript
{
public:
  virtual ~IScript() = default;
  virtual void Update(FlexEngine::FlexECS::Manager&) = 0;
};