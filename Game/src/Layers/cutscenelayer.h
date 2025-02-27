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
        FlexECS::Scene::StringIndex  m_currDialogueFile;
        FlexECS::Scene::StringIndex  m_currCutsceneFile;

        // A container for storing image strings
        std::vector<FlexECS::Scene::StringIndex> m_CutsceneImages;
        std::vector<std::vector<FlexECS::Scene::StringIndex>> m_CutsceneDialogue;

        size_t m_currSectionIndex = 0; // Current section of the cutscene -> keeps track of which portion of the vector of dialogue is currently on play
        size_t m_currFrameIndex = 0; // Current frame of section of the cutscene
        size_t m_currDialogueIndex = 0; // Current dialogue of section of the cutscene
        
        FlexECS::Entity m_currShot;
        FlexECS::Entity m_nextShot;
        FlexECS::Entity m_dialoguebox;
        FlexECS::Entity m_shadowdialoguebox;

        // Timing and phase management.
        float m_ElapsedTime = 0.0f;           // Time spent in normal (non-transition) phase.
        float m_ImageDuration = 0.6f;         // How long to display each image (in seconds).
        float m_PerFrameDuration = 0.0f;      // Duration for each individual frame in the current section.
        int m_frameCount=1;

        float m_dialogueTimer = 0.0f;               // Accumulates time for text animation.
        float m_dialogueTextRate = 30.0f;           // Characters per second to animate.

        bool is_autoplay = false;

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

        void loadCutscene(FlexECS::Scene::StringIndex dialogue_file, FlexECS::Scene::StringIndex cutscene_file);
        void UpdateTimings(bool toNextSection = false);
        
        // Control functions (could be called externally)
        void StartCutscene();
        void StopCutscene();
        void RestartCutscene();
    };

}
