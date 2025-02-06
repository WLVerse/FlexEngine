/*!************************************************************************
* CUTSCENE LAYER [https://wlverse.web.app]
* cutscenelayer.cpp
*
* This file implements the CutsceneLayer class, which manages the playback
* of cutscenes by automatically cycling through a series of images with
* configurable transition effects. The class supports fade-out and fade-in
* transitions between images, enabling smooth visual progression during
* cutscenes.
*
* Key functionalities include:
* - Loading and storing cutscene image identifiers.
* - Automatically swapping between the current and next images based on elapsed time.
* - Managing distinct transition phases (pre-transition and post-transition)
*   to implement fade effects.
*
* The design emphasizes modularity and flexibility, making it easy to integrate
* and extend the cutscene functionality within the game engine's layer system.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Developed the core cutscene sequencing and transition functionalities.
*
* Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/
#include "Layers.h"

namespace Game
{
    /*!
    * \brief Initializes the CutsceneLayer by loading images and creating shot entities.
    *
    * This function is called when the layer is attached to the engine. It sets up
    * the cutscene by loading the image paths, creating the current and next shot
    * entities, and initializing their components.
    */
    void CutsceneLayer::OnAttach()
    {
        // Load cutscene images.
        m_CutsceneImages.push_back(FLX_STRING_NEW(R"(/cutscenes/Cutscene_01.png)"));
        m_CutsceneImages.push_back(FLX_STRING_NEW(R"(/cutscenes/Cutscene_02.png)"));
        m_CutsceneImages.push_back(FLX_STRING_NEW(R"(/cutscenes/Cutscene_03.png)"));

        m_currIndex = 0;

        // Create the current shot entity.
        // Note: Sprite handle is intentionally set to 0 initially.
        m_currShot = FlexECS::Scene::GetActiveScene()->CreateEntity("Current Cinematic Shot");
        m_currShot.AddComponent<Position>({});
        m_currShot.AddComponent<Rotation>({});
        m_currShot.AddComponent<Scale>({ Vector3{ static_cast<float>(Application::GetCurrentWindow()->GetWidth()),
                                                   static_cast<float>(Application::GetCurrentWindow()->GetHeight()), 0.0f } });
        m_currShot.AddComponent<Sprite>({ 0 });
        m_currShot.AddComponent<Transform>({ true, Matrix4x4::Identity, true });
        m_currShot.AddComponent<ZIndex>({ 10 });

        // Create the next shot entity and assign an image if available.
        m_nextShot = FlexECS::Scene::GetActiveScene()->CreateEntity("Next Cinematic Shot");
        m_nextShot.AddComponent<Position>({});
        m_nextShot.AddComponent<Rotation>({});
        m_nextShot.AddComponent<Scale>({ Vector3{ static_cast<float>(Application::GetCurrentWindow()->GetWidth()),
                                                   static_cast<float>(Application::GetCurrentWindow()->GetHeight()), 0.0f } });
        if (m_currIndex + 1 < m_CutsceneImages.size())
            m_nextShot.AddComponent<Sprite>({ m_CutsceneImages[m_currIndex] });
        else
            m_nextShot.AddComponent<Sprite>({ 0 });
        m_nextShot.AddComponent<Transform>({ true, Matrix4x4::Identity, true });
        m_nextShot.AddComponent<ZIndex>({ 9 });

        // Reset timers and transition states.
        m_ElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;
        m_TransitionElapsedTime = 0.0f;

        m_CutsceneActive = false;

        StartCutscene();
    }

    /*!
    * \brief Detaches the CutsceneLayer and performs necessary cleanup.
    *
    * This function is called when the layer is removed from the engine.
    * Add any cleanup logic here if needed.
    */
    void CutsceneLayer::OnDetach()
    {
        // Cleanup code, if needed.
    }

    /*!
    * \brief Activates the cutscene playback.
    *
    * This function sets the cutscene as active and resets all timing and transition states.
    */
    void CutsceneLayer::StartCutscene()
    {
        m_CutsceneActive = true;
        // Reset timers and transition phase.
        m_ElapsedTime = 0.0f;
        m_TransitionElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;
    }

    /*!
    * \brief Deactivates the cutscene playback.
    *
    * Stops the cutscene, disables shot transforms, and sends a message to start the game.
    */
    void CutsceneLayer::StopCutscene()
    {
        m_CutsceneActive = false;

        m_currShot.GetComponent<Transform>()->is_active = false;
        m_nextShot.GetComponent<Transform>()->is_active = false;
        // Additional logic can be added here.
        Application::MessagingSystem::Send("Start Game", true);
    }

    /*!
    * \brief Restarts the cutscene from the beginning.
    *
    * This function resets the image index, timers, and transition states,
    * then reinitializes the shot entities with the appropriate images.
    */
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

    /*!
    * \brief Updates the cutscene state each frame.
    *
    * Handles input for skipping, manages the timing for image display, and
    * processes transition phases for fade effects between images.
    */
    void CutsceneLayer::Update()
    {
        if (m_currShot == FlexECS::Entity::Null || m_nextShot == FlexECS::Entity::Null)
        {
            Log::Debug("Cutscene Shots have been deleted. Please do not delete them.");
            return;
        }

        if (!m_CutsceneActive)
            return;

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
            // Normal display phase: increment elapsed time.
            if (m_currIndex == 0 || m_currIndex > m_CutsceneImages.size())
                m_ElapsedTime = m_ImageDuration;

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
            // Pre-transition phase: fade out the current shot.
            m_TransitionElapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            float progress = m_TransitionElapsedTime / m_PreTransitionDuration;
            float newOpacity = FlexMath::Lerp(1.0f, 0.0f, progress);
            m_currShot.GetComponent<Sprite>()->opacity = newOpacity;

            if (m_TransitionElapsedTime >= m_PreTransitionDuration)
            {
                // Pre-transition complete; swap images.
                SwapShots();

                // Begin the post-transition phase (fade in the new shot).
                m_TransitionPhase = TransitionPhase::PostTransition;
                m_TransitionElapsedTime = 0.0f;
            }
        }
        else if (m_TransitionPhase == TransitionPhase::PostTransition)
        {
            // Post-transition phase: fade in the new shot.
            m_TransitionElapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

            if (m_TransitionElapsedTime >= m_PostTransitionDuration)
            {
                // Transition complete; resume normal display.
                m_TransitionPhase = TransitionPhase::None;
                m_ElapsedTime = 0.0f;
            }
        }
    }

    /*!
    * \brief Swaps the current shot with the next shot during a transition.
    *
    * This function is called after the pre-transition phase is complete.
    * It updates the sprite handle and opacity of the current shot, advances
    * the cutscene image index, and prepares the next shot with the subsequent image.
    */
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
} // namespace Game
