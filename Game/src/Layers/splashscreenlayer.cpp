/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// splashscreenlayer.cpp
//
// Implements SplashScreenLayer which manages the editor splash screen.
// Loads splash scene, fades logo in/out, initializes and ramps post-processing
// effects, and handles transition messaging.
//
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "FlexEngine.h"
#include "Layers.h"

namespace Game
{
    // Function: SplashScreenLayer::OnAttach
    // Description: Load splash scene, set active scene and camera, cache logo entity,
    //              reset logo opacity, elapsed timers, and flags.
    void SplashScreenLayer::OnAttach()
    {
        File& file = File::Open(Path::current("assets/saves/DigipenSplashScreen.flxscene"));
        FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
        CameraManager::TryMainCamera();

        auto activeScene = FlexECS::Scene::GetActiveScene();
        m_logo = activeScene->GetEntityByName("Logo");
        m_logo.GetComponent<Sprite>()->opacity = 0.0f;

        m_logoFadeElapsed = 0.0f;
        m_totalElapsed = 0.0f;
        m_postProcessInitialized = false;
        m_transitionSent = false;
        m_logoFadedIn = false;
        m_logoFadedOut = false;
    }

    // Function: SplashScreenLayer::OnDetach
    // Description: Cleanup when layer is detached. Currently no actions required.
    void SplashScreenLayer::OnDetach()
    {
    }

    // Function: SplashScreenLayer::Update
    // Description: Per-frame update to fade logo in, initialize post-processing
    //              once, ramp chromatic aberration and bloom effects, send
    //              transition start message, and handle transition completion.
    void SplashScreenLayer::Update()
    {
        auto activeScene = FlexECS::Scene::GetActiveScene();
        if (!activeScene)
            return;

        float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // Fade in logo.
        if (!m_logoFadedIn)
        {
            m_logoFadeElapsed += dt;
            float fadeProgress = std::min(m_logoFadeElapsed / m_fadeInDuration, 1.0f);
            m_logo.GetComponent<Sprite>()->opacity = fadeProgress;
            if (fadeProgress >= 1.0f)
                m_logoFadedIn = true;
            return;
        }

        // Initialize postprocessing once after fade in.
        if (!m_postProcessInitialized)
        {
            for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
            {
                auto transform = element.GetComponent<Transform>();
                if (!transform || !transform->is_active)
                    continue;
                if (!element.HasComponent<PostProcessingChromaticAbberation>())
                    element.AddComponent<PostProcessingChromaticAbberation>({});
                if (auto aberration = element.GetComponent<PostProcessingChromaticAbberation>())
                {
                    aberration->intensity = m_chromaticStartIntensity;
                    aberration->redOffset = { 0.0f, 0.0f };
                    aberration->greenOffset = { 0.0f, 0.0f };
                    aberration->blueOffset = { 0.0f, 0.0f };
                    aberration->edgeSoftness = { 9999.0f, 9999.0f };
                    aberration->edgeRadius = { 9999.0f, 9999.0f };
                }
                if (!element.HasComponent<PostProcessingBloom>())
                    element.AddComponent<PostProcessingBloom>({});
                if (auto bloom = element.GetComponent<PostProcessingBloom>())
                    bloom->intensity = m_bloomStart;
                element.GetComponent<PostProcessingMarker>()->enableChromaticAberration = true;
                element.GetComponent<PostProcessingMarker>()->enableBloom = true;
            }
            m_totalElapsed = 0.0f;
            m_postProcessInitialized = true;
        }

        // Ramp up effects (continues even after transition starts).
        m_totalElapsed += dt;
        float normalizedProgress = std::min(m_totalElapsed / m_effectRampDuration, 1.0f);

        for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
        {
            auto transform = element.GetComponent<Transform>();
            if (!transform || !transform->is_active)
                continue;

            if (auto aberration = element.GetComponent<PostProcessingChromaticAbberation>())
            {
                aberration->intensity = FlexMath::Lerp(
                    m_chromaticStartIntensity,
                    m_chromaticTargetIntensity,
                    normalizedProgress
                );
                float amount = static_cast<float>(
                    rand() % static_cast<int>(
                        FlexMath::Lerp(1.0f, m_glitchMaxOffset, normalizedProgress)
                        )
                );
                aberration->redOffset.x = amount;
                aberration->greenOffset.x = 0.0f;
                aberration->blueOffset.x = -amount;
            }
            if (auto bloom = element.GetComponent<PostProcessingBloom>())
            {
                float bloomDelay = m_effectRampDuration * 0.15f;
                float bloomIntensity = m_bloomStart;
                if (m_totalElapsed > bloomDelay)
                {
                    float bloomProgress = (m_totalElapsed - bloomDelay)
                        / (m_effectRampDuration - bloomDelay);
                    bloomIntensity = FlexMath::Lerp(
                        m_bloomStart,
                        m_bloomTarget,
                        std::min(bloomProgress, 1.0f)
                    );
                }
                bloom->intensity = bloomIntensity;
            }
        }

        // Trigger transition at 25% of effect duration.
        if (m_totalElapsed >= m_effectRampDuration * 0.25f && !m_transitionSent)
        {
            Application::MessagingSystem::Send(
                "TransitionStart",
                std::pair<int, double>{ 2, 1.0 }
            );
            m_transitionSent = true;
        }

        // Wait for transition completion.
        if (m_transitionSent && !m_logoFadedOut)
        {
            int transitionMSG = Application::MessagingSystem::Receive<int>(
                "TransitionCompleted"
            );
            if (transitionMSG == 2)
            {
                Application::MessagingSystem::Send("Start Menu", true);
                m_logoFadedOut = true;
            }
        }
    }
} // namespace Game
