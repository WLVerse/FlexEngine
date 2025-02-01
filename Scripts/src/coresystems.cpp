#include <FlexEngine.h>
#include "FlexEngine/Physics/physicssystem.h"

using namespace FlexEngine;

class CoreSystemsScript : public IScript
{
public:
  CoreSystemsScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "GameplayLoops"; }

  void Awake() override
  {

  }

  void Start() override
  {
    
  }

  void Update() override
  {
    
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static CoreSystemsScript GameplayLoops;
