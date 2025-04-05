// WLVERSE [https://wlverse.web.app]
// endingcutscenelayer.cpp
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

#include "Layers.h"

//Not using transitions for going to main menu, because main menu also
//relies on transitions to go to start game,
//and our messaging system doesn't clean input.

namespace Game
{
  void EndingCutsceneLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/endingcutscene.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    // Play cutscene audio
    FlexECS::Scene::GetEntityByName("CutsceneAudio").GetComponent<Audio>()->should_play = true;

    m_skiptext = FlexECS::Scene::GetEntityByName("Skip Text");
    m_instructiontxt = FlexECS::Scene::GetEntityByName("Instruction Text");
    m_instructiontxtopacityblk = FlexECS::Scene::GetEntityByName("Instruction Text Opacity Block");
    m_skipwheel = FlexECS::Scene::GetEntityByName("Skip Wheel");

    auto& font = FLX_ASSET_GET(Asset::Font, R"(/fonts/Electrolize/Electrolize-Regular.ttf)");
    font.SetFontSize(30);
  
    m_videoplayer = FlexECS::Scene::GetEntityByName("VideoPlayer");
    auto& videoplayer = *m_videoplayer.GetComponent<VideoPlayer>();
    auto& video = FLX_ASSET_GET(VideoDecoder, FLX_STRING_GET(videoplayer.video_file));
    video.RestartVideo();
  }

  void EndingCutsceneLayer::OnDetach()
  {
    auto& font = FLX_ASSET_GET(Asset::Font, R"(/fonts/Electrolize/Electrolize-Regular.ttf)");
    font.SetFontSize(50);
  }

  void EndingCutsceneLayer::Update()
  {
    int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
    if (transitionMSG == 2)
    {
      Input::Cleanup();
      Application::MessagingSystem::Send("Ending Cutscene to Menu", true);
    }

    auto& videoplayer = *m_videoplayer.GetComponent<VideoPlayer>();
    auto& video = FLX_ASSET_GET(VideoDecoder, FLX_STRING_GET(videoplayer.video_file));

    if (video.m_current_time >= video.GetLength() && !m_messageSent)
    {
      //Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 5, 0.5 });
      Application::MessagingSystem::Send("Ending Cutscene to Menu", true);
      m_messageSent = true;  // Mark that the message has been sent.
    }

    updateInstructionAnimation();
    UpdateSkipUI();
  }

  void EndingCutsceneLayer::updateInstructionAnimation()
  {
    if (!m_instructionActive)
      return;

    float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

    if (Input::GetKey(GLFW_KEY_ESCAPE) || m_instructionTimer >= m_instructionDuration)
    {
      m_instructiontxt.GetComponent<Transform>()->is_active = false;
      m_instructiontxtopacityblk.GetComponent<Sprite>()->opacity = 1.0f;
      m_instructionActive = false;
      return;
    }

    m_instructionTimer += dt;
    float progress = m_instructionTimer / m_instructionDuration; // instructionDuration is a set duration
    auto* pos = m_instructiontxt.GetComponent<Position>();
    pos->position.x = FlexMath::Lerp(pos->position.x, pos->position.x + 1, progress); // Just move to the right a bit
    auto* sprite = m_instructiontxtopacityblk.GetComponent<Sprite>();
    sprite->opacity = FlexMath::Lerp(0.0f, 1.0f, progress);
  }

  void EndingCutsceneLayer::UpdateSkipUI()
  {
    float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
    if (Input::GetKey(GLFW_KEY_ESCAPE))
    {
      m_skipTimer += dt;
      // Animate skip text: "Commencing Skip"
      static std::string fullSkipText = "Commencing Skip";
      size_t totalChars = fullSkipText.size();
      size_t charsToShow = static_cast<size_t>(m_skipTimer * m_skipTextRate);
      if (charsToShow > totalChars)
        charsToShow = totalChars;
      std::string displayedText = fullSkipText.substr(0, charsToShow);
      m_skiptext.GetComponent<Text>()->text = FLX_STRING_NEW(displayedText.c_str());

      // Animate skip wheel opacity and rotation.
      auto* skipWheelSprite = m_skipwheel.GetComponent<Sprite>();
      skipWheelSprite->opacity = 1.0f;
      auto* skipWheelrotation = m_skipwheel.GetComponent<Rotation>();
      // Increase rotation speed based on skipTimer.
      skipWheelrotation->rotation.z -= (m_baseRotationSpeed * m_skipTimer * dt);

      // Animate opacity of skip wheel.
      m_instructiontxtopacityblk.GetComponent<Sprite>()->opacity = FlexMath::Lerp(1.0f, 0.0f, m_skipTimer / m_skipFadeDuration);

      // Check if held down long enough to trigger stop, and only send once.
      if (m_skipTimer >= m_skipHoldThreshold && !m_messageSent)
      {
        //Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 5, 0.5 });
        Application::MessagingSystem::Send("Ending Cutscene to Menu", true);
        m_messageSent = true;  // Mark that the message has been sent.
      }
    }
    else
    {
      // Reset skip UI if ESC is released.
      m_skipTimer = 0.0f;
      m_skiptext.GetComponent<Text>()->text = FLX_STRING_NEW("");
      m_skipwheel.GetComponent<Sprite>()->opacity = 0;
      m_skipwheel.GetComponent<Rotation>()->rotation.z = 0.0f;
      m_messageSent = false; // Reset flag so that next press can send the message.
    }
  }

}
