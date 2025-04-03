// WLVERSE [https://wlverse.web.app]
// endingcutscenelayer.h
// 
// Plays ending cutscene.
//
// AUTHORS
// [30%] Rocky (rocky.sutarius\@digipen.edu)
//   - Main Author
//  most of this file is copy pasted code done by 
// Soh Wei Jie (weijie.soh\@digipen.edu)
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{

  class EndingCutsceneLayer : public FlexEngine::Layer
  {
  public:
    EndingCutsceneLayer() : Layer("EndingCutscene Layer") {}
    ~EndingCutsceneLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void updateInstructionAnimation();
    void UpdateSkipUI();

    FlexECS::Entity m_videoplayer;

    FlexECS::Entity m_skiptext;
    FlexECS::Entity m_instructiontxt;
    FlexECS::Entity m_instructiontxtopacityblk; //Have to do this due to text no opacity
    FlexECS::Entity m_skipwheel;     // To show that auto playing isn't active



    // UI timers
    bool m_instructionActive = true;
    float m_instructionTimer = 0.0f;
    float m_instructionDuration = 2.0f;
    bool m_dialogueIsWaitingForInput = false;

    float m_skipTimer = 0.0f;
    float m_skipFadeDuration = 2.0f;
    float m_skipTextRate = 20.0f;
    float m_baseRotationSpeed = 1200.0f;
    float m_skipHoldThreshold = 3.0f;


    // Overall cutscene activation flag.
    bool m_CutsceneActive = false;
    bool m_messageSent = false;
  };

}
