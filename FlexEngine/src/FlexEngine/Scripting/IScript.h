#include "FlexECS/datastructures.h"

class IScript
{
public:
  virtual ~IScript() = default;
  virtual void Update(FlexEngine::FlexECS::Manager&) = 0;
};

// Function pointer types for creating scripts
using CreateScriptFunction = IScript* (*)();