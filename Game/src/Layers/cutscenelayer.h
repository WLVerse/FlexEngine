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

    class CutsceneLayer : public FlexEngine::Layer
    {
        // Asset references.
        FlexECS::Scene::StringIndex  m_currDialogueFile;
        FlexECS::Scene::StringIndex  m_currCutsceneFile;

        // Containers for cutscene images and dialogue.
        std::vector<FlexECS::Scene::StringIndex> m_CutsceneImages;
        std::vector<std::vector<FlexECS::Scene::StringIndex>> m_CutsceneDialogue;
        
        #pragma region Variables

        // Index management for sections, frames, and dialogue lines.
        size_t m_currSectionIndex = 0;   // Which cutscene section is active.
        size_t m_currFrameIndex = 0;     // Which frame in the current section.
        size_t m_currDialogueIndex = 0;  // Which dialogue line is active.

        // Entities used for rendering.
        FlexECS::Entity m_currShot;
        FlexECS::Entity m_nextShot;
        FlexECS::Entity m_dialoguebox;
        FlexECS::Entity m_shadowdialoguebox;
        
        // Timing and phase management.
        float m_ElapsedTime = 0.0f;      // Time in the normal (non-transition) phase.
        float m_ImageDuration = 0.6f;    // Duration for displaying each image.
        float m_PerFrameDuration = 0.0f; // Duration for each frame in the section.
        int m_frameCount = 1;

        float m_dialogueTimer = 0.0f;    // Accumulates time for text animation.
        float m_dialogueHoldTimer = 0.0f;// Time after text is fully revealed.
        float m_dialogueTextRate = 30.0f;// Characters per second.
        float m_dialogueHoldDuration = 1.0f; // Hold duration after full text reveal.
        bool is_autoplay = false;         // Determines if dialogue advances automatically.

        // Transition phase variables.
        TransitionPhase m_TransitionPhase = TransitionPhase::None;
        float m_TransitionElapsedTime = 0.0f;
        float m_PreTransitionDuration = 1.0f;
        float m_PostTransitionDuration = 1.0f;

        // Overall cutscene activation flag.
        bool m_CutsceneActive = false;

        #pragma endregion

        #pragma region Helper Func
    private:
        // Process global inputs such as restarting or exiting the cutscene.
        void processGlobalInput();

        // Update the dialogue text animation (common to both auto and manual modes).
        void updateDialogueText(float dt);

        // Advance to the next dialogue line and trigger transition if needed.
        void advanceDialogue();

        // Skip remaining frames in the current section.
        void skipRemainingFrames();

        // Update dialogue for auto-run mode.
        void updateDialogueAuto(float dt);

        // Update dialogue for manual (user-controlled) mode.
        void updateDialogueManual(float dt);

        // Update image frames for multi-frame sections.
        void updateImageFrames(float dt);

        // Handle the transition effects (fade-out/in).
        void updateTransitionPhase(float dt);

        // Swap current and next shot entities.
        void SwapShots();

        void UpdateTimings(bool toNextSection = false);

        #pragma endregion

    public:
        CutsceneLayer() : Layer("Cutscene Layer") {}
        ~CutsceneLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update() override;

        void loadCutscene(FlexECS::Scene::StringIndex dialogue_file, FlexECS::Scene::StringIndex cutscene_file);
       
        // Control functions (can be called externally).
        void StartCutscene();
        void StopCutscene();
        void RestartCutscene();
    };

}
