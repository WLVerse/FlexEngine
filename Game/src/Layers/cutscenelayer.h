// WLVERSE [https://wlverse.web.app]
// baselayer.h
// 
// Base layer that runs before everything.
//
// AUTHORS
// [100%] Soh Wei Jie
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    enum TransitionPhase
    {
        None,          // No transition is occurring; normal image display.
        PreTransition, // Pre-transition phase (e.g., fade-out before swap).
        PostTransition // Post-transition phase (e.g., fade-in after swap).
    };

    //A basic auto Carousel typed cutscene layer -> TODO develop functionality for dialogue
    class CutsceneLayer : public FlexEngine::Layer
    {
        // A container for storing image strings
        std::vector<FlexECS::Scene::StringIndex> m_CutsceneImages;

        size_t m_currIndex = 0;

        FlexECS::Entity m_currShot;
        FlexECS::Entity m_nextShot;

        // Timing and phase management.
        float m_ElapsedTime = 0.0f;           // Time spent in normal (non-transition) phase.
        float m_ImageDuration = 0.6f;         // How long to display each image (in seconds).

        // Transition phase (for effects like fade out/in).
        TransitionPhase m_TransitionPhase = TransitionPhase::None;
        float m_TransitionElapsedTime = 0.0f; // Time accumulator for transition phases.
        float m_PreTransitionDuration = 1.0f; // Duration (in seconds) of pre-transition phase.
        float m_PostTransitionDuration = 1.0f; // Duration (in seconds) of post-transition phase.

        // Overall cutscene activation flag.
        bool m_CutsceneActive = false;

        // Function to handle transition between shots 
        // (currShot is being rendered on top of next shot)
        void SwapShots();
    public:
        CutsceneLayer() : Layer("Cutscene Layer") {}
        ~CutsceneLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update() override;

        // Control functions (could be called externally)
        void StartCutscene();
        void StopCutscene();
        void RestartCutscene();
    };

}
