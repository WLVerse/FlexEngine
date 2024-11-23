// ScriptingInterface.h
#ifndef SCRIPTING_INTERFACE_H
#define SCRIPTING_INTERFACE_H

#include "FlexECS/datastructures.h"

class IScript {
public:
  virtual ~IScript() = default;
  virtual void onUpdate(std::shared_ptr<FlexEngine::FlexECS::Scene> ecs) = 0;
};

// Function pointer types for creating scripts
using CreateScript1Func = IScript * (*)();
using CreateScript2Func = IScript * (*)();

#endif // SCRIPTING_INTERFACE_H