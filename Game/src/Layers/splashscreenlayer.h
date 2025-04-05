#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    class SplashScreenLayer : public FlexEngine::Layer
    {
        float m_totalElapsed = 0.0f;
        bool m_postProcessInitialized = false;
        bool m_transitionSent = false; // Ensures transition is sent only once.

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

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update() override;
    };
}
