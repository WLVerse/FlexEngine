/*!************************************************************************
* CUTSCENE LAYER [https://wlverse.web.app]
* cutscenelayer.h
*
* This file implements the CutsceneLayer class, which manages the playback
* of cutscenes by automatically cycling through a series of images with
* configurable transition effects. The class supports fade-out and fade-in
* transitions between images, allowing for smooth visual progression during
* cutscenes.
*
* Key functionalities include:
* - Maintaining a list of cutscene image identifiers.
* - Automatically swapping the current and next images based on elapsed time.
* - Handling distinct phases of transition (pre-transition and post-transition)
*   to provide fade effects.
*
* The design focuses on modularity and flexibility, enabling easy integration
* and extension within the game engine's layer system.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Developed the core cutscene sequencing and transition functionalities.
*
* Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    /// \brief Enumeration to represent the different phases of a cutscene transition.
    enum TransitionPhase
    {
        None,          ///< No transition is occurring; normal image display.
        PreTransition, ///< Pre-transition phase (e.g., fade-out before swap).
        PostTransition ///< Post-transition phase (e.g., fade-in after swap).
    };

    /*!
    * \class CutsceneLayer
    * \brief A layer that handles auto-carouseling cutscenes with image transitions.
    *
    * This class is responsible for rendering a sequence of cutscene images and
    * managing the transitions between them. It uses configurable timers to
    * determine how long each image is displayed and to control fade effects during
    * transitions.
    */
    class CutsceneLayer : public FlexEngine::Layer
    {
        // Container for storing image identifiers for the cutscene.
        std::vector<FlexECS::Scene::StringIndex> m_CutsceneImages;

        // Current index in the image list.
        size_t m_currIndex = 0;

        // Entities representing the current and next shots.
        FlexECS::Entity m_currShot;
        FlexECS::Entity m_nextShot;

        // Timing and phase management.
        float m_ElapsedTime = 0.0f;           ///< Time spent in normal (non-transition) phase.
        float m_ImageDuration = 0.6f;         ///< Duration (in seconds) each image is displayed.

        // Transition phase management.
        TransitionPhase m_TransitionPhase = TransitionPhase::None;
        float m_TransitionElapsedTime = 0.0f; ///< Accumulated time during transition phases.
        float m_PreTransitionDuration = 1.0f; ///< Duration (in seconds) of the pre-transition phase.
        float m_PostTransitionDuration = 1.0f;///< Duration (in seconds) of the post-transition phase.

        // Overall flag to indicate if the cutscene is active.
        bool m_CutsceneActive = false;

        /*!
        * \brief Handles the transition between the current shot and the next shot.
        *
        * This function performs the necessary operations to swap images during a
        * transition phase, ensuring that the current shot is replaced by the next shot.
        */
        void SwapShots();

    public:
        /*!
        * \brief Constructor for the CutsceneLayer.
        *
        * Initializes the layer with its designated name and default settings.
        */
        CutsceneLayer() : Layer("Cutscene Layer") {}

        /*!
        * \brief Default destructor for the CutsceneLayer.
        */
        ~CutsceneLayer() = default;

        /*!
        * \brief Called when the layer is attached to the game engine.
        *
        * Performs initialization tasks such as resource setup and loading of
        * cutscene images.
        */
        virtual void OnAttach() override;

        /*!
        * \brief Called when the layer is detached from the game engine.
        *
        * Cleans up any resources or states associated with the cutscene.
        */
        virtual void OnDetach() override;

        /*!
        * \brief Updates the cutscene layer each frame.
        *
        * Handles the timing for image display and transition phases, and initiates
        * image swaps as needed.
        */
        virtual void Update() override;

        /*!
        * \brief Starts the cutscene playback.
        *
        * Activates the cutscene sequence, beginning the image cycling and transition effects.
        */
        void StartCutscene();

        /*!
        * \brief Stops the cutscene playback.
        *
        * Deactivates the cutscene, resetting timers and transition states.
        */
        void StopCutscene();

        /*!
        * \brief Restarts the cutscene from the beginning.
        *
        * Resets the cutscene sequence and all associated timing parameters, then starts playback.
        */
        void RestartCutscene();
    };

} // namespace Game