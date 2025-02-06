// WLVERSE [https://wlverse.web.app]
// playanimation.cpp
//
// Scripting cpp file for testing animation playing. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include <FlexEngine.h>
using namespace FlexEngine;

class PlayAnimationScript : public IScript
{
public:
  PlayAnimationScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "PlayAnimation"; }

  void Awake() override
  {

  }

  void Start() override
  {
    Log::Info("PlayAnimationScript attached to entity: " + FLX_STRING_GET(*self.GetComponent<EntityName>()) + ". This script will animate sprites.");

    self.GetComponent<Animator>()->should_play = false;
  }

  void Update() override
  {
    if (Input::AnyKey())
    {
      self.GetComponent<Animator>()->should_play = true;
    }
    else
    {
      self.GetComponent<Animator>()->should_play = false;
    }
  }

  void Stop() override
  {
    self.GetComponent<Animator>()->should_play = true;
  }
};

// Static instance to ensure registration
static PlayAnimationScript PlayAnimation;