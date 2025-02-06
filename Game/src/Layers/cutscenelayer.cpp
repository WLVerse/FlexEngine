// WLVERSE [https://wlverse.web.app]
// baselayer.cpp
// 
// Base layer that runs before everything.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Game
{

    void CutsceneLayer::OnAttach()
    {
        // Load cutscene images.
        m_CutsceneImages.push_back(FLX_STRING_NEW(R"(/cutscenes/Cutscene_01.png)"));
        m_CutsceneImages.push_back(FLX_STRING_NEW(R"(/cutscenes/Cutscene_02.png)"));
        m_CutsceneImages.push_back(FLX_STRING_NEW(R"(/cutscenes/Cutscene_03.png)"));

        m_currIndex = 0;

        // Create the current shot entity.
        // Intentionally setting sprite handle to 0.
        m_currShot = FlexECS::Scene::GetActiveScene()->CreateEntity("Current Cinematic Shot");
        m_currShot.AddComponent<Position>({});
        m_currShot.AddComponent<Rotation>({});
        m_currShot.AddComponent<Scale>({ Vector3{static_cast<float>(Application::GetCurrentWindow()->GetWidth()),
                                                 static_cast<float>(Application::GetCurrentWindow()->GetHeight()), 0.0f } });
        m_currShot.AddComponent<Sprite>({ 0 });
        m_currShot.AddComponent<Transform>({ true, Matrix4x4::Identity, true });
        m_currShot.AddComponent<ZIndex>({ 10 });

        // Create the next shot entity and assign the image if available.
        m_nextShot = FlexECS::Scene::GetActiveScene()->CreateEntity("Next Cinematic Shot");
        m_nextShot.AddComponent<Position>({});
        m_nextShot.AddComponent<Rotation>({});
        m_nextShot.AddComponent<Scale>({ Vector3{static_cast<float>(Application::GetCurrentWindow()->GetWidth()),
                                                 static_cast<float>(Application::GetCurrentWindow()->GetHeight()), 0.0f } });
        if (m_currIndex + 1 < m_CutsceneImages.size())
            m_nextShot.AddComponent<Sprite>({ m_CutsceneImages[m_currIndex] });
        else
            m_nextShot.AddComponent<Sprite>({ 0 });
        m_nextShot.AddComponent<Transform>({ true, Matrix4x4::Identity, true });
        m_nextShot.AddComponent<ZIndex>({ 9 });

        m_ElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;
        m_TransitionElapsedTime = 0.0f;

        m_CutsceneActive = false;

        StartCutscene();
    }

    void CutsceneLayer::OnDetach()
    {
        // Cleanup code, if needed.
    }

    void CutsceneLayer::StartCutscene()
    {
        m_CutsceneActive = true;
        // Reset timers and transition phase.
        m_ElapsedTime = 0.0f;
        m_TransitionElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;
    }

    void CutsceneLayer::StopCutscene()
    {
        m_CutsceneActive = false;

        //Additional logic
        Application::MessagingSystem::Send("Start Game", true);
    }

    void CutsceneLayer::RestartCutscene()
    {
        // Reset indices and timers.
        m_currIndex = 0;
        m_ElapsedTime = 0.0f;
        m_TransitionElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;

        // Set up the current shot with the first image.
        m_currShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currIndex];
        // Set up the next shot with the second image if available.
        if (m_currIndex + 1 < m_CutsceneImages.size())
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currIndex + 1];
        else
            m_nextShot.GetComponent<Sprite>()->sprite_handle = 0;

        m_CutsceneActive = true;
    }

    void CutsceneLayer::Update()
    {
        if (m_currShot == FlexECS::Entity::Null || m_nextShot == FlexECS::Entity::Null)
        {
            Log::Debug("Cutscene Shots have been deleted. Please do not delete them.");
            return;
        }


        if (!m_CutsceneActive)
            return;

        // Debug output for transition phases.
        #if 0
        if (m_TransitionPhase == TransitionPhase::PreTransition)
            std::cout << "PRE" << std::endl;
        if (m_TransitionPhase == TransitionPhase::PostTransition)
            std::cout << "POST" << std::endl;
        #endif

        // --- Input Handling for skipping ---
        if (Input::GetKey(GLFW_KEY_SPACE))
        {
            // Immediately initiate a pre-transition.
            m_TransitionPhase = TransitionPhase::PreTransition;
            m_TransitionElapsedTime = 0.0f;
        }
        else if (Input::GetKey(GLFW_KEY_ESCAPE))
        {
            // Stop the cutscene.
            StopCutscene();
            return;
        }

        // --- Phase-Based Update ---
        if (m_TransitionPhase == TransitionPhase::None)
        {
            if (m_currIndex == 0 || m_currIndex > m_CutsceneImages.size())
                m_ElapsedTime = m_ImageDuration;

            // Normal display phase.
            m_ElapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            if (m_ElapsedTime >= m_ImageDuration)
            {
                // Time to start the pre-transition.
                m_TransitionPhase = TransitionPhase::PreTransition;
                m_TransitionElapsedTime = 0.0f;
            }
        }
        else if (m_TransitionPhase == TransitionPhase::PreTransition)
        {
            // Pre-transition phase (e.g., fading out the current shot).
            m_TransitionElapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            float progress = m_TransitionElapsedTime / m_PreTransitionDuration;
            float newOpacity = FlexMath::Lerp(1.0f, 0.05f, progress);
            m_currShot.GetComponent<Sprite>()->opacity = newOpacity;

            if (m_TransitionElapsedTime >= m_PreTransitionDuration)
            {
                // Pre-transition complete; swap images.
                SwapShots();

                // Start the post-transition phase (e.g., fading in the new shot).
                m_TransitionPhase = TransitionPhase::PostTransition;
                m_TransitionElapsedTime = 0.0f;
            }
        }
        else if (m_TransitionPhase == TransitionPhase::PostTransition)
        {
            // Post-transition phase (e.g., fading in the new shot).
            m_TransitionElapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

            if (m_TransitionElapsedTime >= m_PostTransitionDuration)
            {
                // Transition complete; resume normal display.
                m_TransitionPhase = TransitionPhase::None;
                m_ElapsedTime = 0.0f;
            }
        }
    }

    void CutsceneLayer::SwapShots()
    {
        if (m_currIndex >= m_CutsceneImages.size() + 1)
        {
            StopCutscene();
            return;
        }

        // The next shot becomes the current shot.
        m_currShot.GetComponent<Sprite>()->sprite_handle = m_nextShot.GetComponent<Sprite>()->sprite_handle;
        m_currShot.GetComponent<Sprite>()->opacity = 1.0f;
        m_currIndex++;

        // Prepare the next shot with the subsequent image, if available.
        if (m_currIndex < m_CutsceneImages.size())
        {
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currIndex];
        }
        else
        {
            m_nextShot.GetComponent<Sprite>()->sprite_handle = 0;
        }
    }
}
