// WLVERSE [https://wlverse.web.app]
// splashscreenlayer.h
//
// Declares the SplashScreenLayer class which manages the editor splash screen.
// Handles chromatic aberration, glitch offset, bloom ramp, logo fade-in/out,
// and sends a transition message when complete.
//
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    // Renders and updates the splash screen sequence
    class SplashScreenLayer : public FlexEngine::Layer
    {
        float m_totalElapsed = 0.0f;
        bool  m_postProcessInitialized = false;
        bool  m_transitionSent = false; // Ensures transition is sent only once.

        // Transition parameters
        float m_effectRampDuration = 10.0f;  // Duration for ramping effects

        // Chromatic aberration parameters
        float m_chromaticStartIntensity = 0.0f;
        float m_chromaticTargetIntensity = 4.0f;
        float m_glitchMaxOffset = 200.0f;

        // Bloom parameters
        float m_bloomStart = 0.0f;
        float m_bloomTarget = 20.0f;

        // Cached logo entity.
        FlexECS::Entity m_logo;
        // Logo fade state flags.
        bool m_logoFadedIn = false;
        bool m_logoFadedOut = false;

        // Fade timing.
        float m_logoFadeElapsed = 0.0f;
        const float m_fadeInDuration = 1.0f;

    public:
        SplashScreenLayer() : Layer("Splash Screen Layer") {}
        ~SplashScreenLayer() = default;

        // Function: SplashScreenLayer::OnAttach
        // Description: Called when the layer is attached; creates the logo entity,
        //              initializes post-processing resources.
        virtual void OnAttach() override;

        // Function: SplashScreenLayer::OnDetach
        // Description: Called when the layer is detached; releases post-processing
        //              resources and removes the logo entity.
        virtual void OnDetach() override;

        // Function: SplashScreenLayer::Update
        // Description: Updates elapsed time each frame, animates chromatic aberration,
        //              glitch offset, bloom intensity, logo fade-in/out, and sends
        //              the transition message when the splash sequence completes.
        virtual void Update() override;
    };
} // namespace Game
