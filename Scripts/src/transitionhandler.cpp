/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// transitionhandler.cpp
//
// Provides transition effects and a script to manage them, including fade-in
// and fade-out effects using easing functions.
//
// This module defines the ITransitionEffect interface, a concrete FadeTransition
// implementation, and the TransitionHandlerScript, which queues and processes
// transitions while sending notifications upon completion.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie_soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
#include <memory>
#include <queue>
using namespace FlexEngine;

namespace Game
{
    #pragma region Transitions

    // Enumerates the types of transitions available.
    enum class TransitionType
    {
        FADE_IN = 1,
        FADE_OUT = 2,

        ENCOUNTER_TRANSITION_BATTLE_IN = 3,
        // Extend with additional types as needed.
    };

    // Abstract interface for transition effects.
    class ITransitionEffect
    {
    public:
        virtual ~ITransitionEffect() {}

        // Initializes the transition effect.
        virtual void Start() = 0;

        // Updates the effect based on the elapsed time.
        // @param dt Time delta since the last update.
        virtual void Update(float dt) = 0;

        // Checks if the effect has completed.
        // @return true if complete; false otherwise.
        virtual bool IsComplete() const = 0;

        // Retrieves the type of the transition effect.
        // @return The TransitionType of the effect.
        virtual TransitionType GetType() const = 0;

        // Stops and cleans up the transition effect.
        virtual void Stop() = 0;
    };

    #pragma region Fade

    // FadeTransition implements a fade effect using an easing (lerp) function.
    class FadeTransition : public ITransitionEffect
    {
    private:
        FlexECS::Entity m_transitionFrame; ///< Entity used to render the fade.
        float m_elapsedTime;               ///< Accumulated time since start.
        float m_duration;                  ///< Total duration of the fade effect.
        float m_initialAlpha;              ///< Starting opacity.
        float m_targetAlpha;               ///< Target opacity.
        float m_currentAlpha;              ///< Current computed opacity.
        bool m_complete;                   ///< Flag indicating effect completion.

    public:
        // Constructs a FadeTransition with the specified parameters.
        // @param duration Total duration of the fade.
        // @param initialAlpha Starting opacity value.
        // @param targetAlpha Ending opacity value.
        FadeTransition(float duration, float initialAlpha, float targetAlpha)
            : m_elapsedTime(0.0f), m_duration(duration),
            m_initialAlpha(initialAlpha), m_targetAlpha(targetAlpha),
            m_currentAlpha(initialAlpha), m_complete(false)
        {
            auto currentScene = FlexECS::Scene::GetActiveScene();
            if (currentScene)
            {
                m_transitionFrame = currentScene->CreateEntity("TransitionFrame");
                m_transitionFrame.AddComponent<Position>({});
                m_transitionFrame.AddComponent<Rotation>({});
                m_transitionFrame.AddComponent<Scale>({ Vector3(9999, 9999, 0) });
                m_transitionFrame.AddComponent<Transform>({});
                m_transitionFrame.AddComponent<ZIndex>({ 9999 });
                m_transitionFrame.AddComponent<Sprite>({});

                auto sprite = m_transitionFrame.GetComponent<Sprite>();
                if (sprite)
                {
                    sprite->opacity = m_currentAlpha;
                }
                else
                {
                    Log::Warning("FadeTransition: Sprite component not found on transition entity.");
                }
            }
            else
            {
                Log::Error("FadeTransition: Active scene not found.");
            }
        }

        // Starts the fade transition. Additional initialization can be added here.
        void Start() override
        {
            // No additional initialization required at this time.
        }

        // Updates the fade effect over time using linear interpolation.
        // @param dt Time delta since the last update.
        void Update(float dt) override
        {
            if (m_complete)
                return;

            m_elapsedTime += dt;
            float progress = m_elapsedTime / m_duration;
            if (progress > 1.0f)
                progress = 1.0f;

            // Compute the current opacity using linear interpolation.
            m_currentAlpha = FlexMath::Lerp(m_initialAlpha, m_targetAlpha, progress);

            auto sprite = m_transitionFrame.GetComponent<Sprite>();
            if (sprite)
            {
                sprite->opacity = m_currentAlpha;
            }
            else
            {
                Log::Warning("FadeTransition: Sprite component missing during update.");
            }

            if (m_elapsedTime >= m_duration)
            {
                m_complete = true;
            }
        }

        // Returns whether the fade effect is complete.
        // @return true if the fade is complete, false otherwise.
        bool IsComplete() const override
        {
            return m_complete;
        }

        // Determines the transition type based on the alpha values.
        // @return FADE_IN if fading in; FADE_OUT if fading out.
        TransitionType GetType() const override
        {
            return (m_targetAlpha < m_initialAlpha) ? TransitionType::FADE_IN : TransitionType::FADE_OUT;
        }

        // Stops the fade effect and resets the transition entity.
        void Stop() override
        {
            auto currentScene = FlexECS::Scene::GetActiveScene();
            if (currentScene && m_transitionFrame)
            {
                // Note: Scene auto-removal is assumed; reset the handle instead.
                m_transitionFrame = FlexECS::Entity();
            }
            else
            {
                Log::Warning("FadeTransition: Unable to stop transition; entity or scene missing.");
            }
        }
    };
    #pragma endregion

    #pragma region Encounter Transition from town to battle

    // EncounterBattleTransition implements a multi-stage transition effect:
    // 1) Camera zooms in to specified ortho width, lerping over time.
    // 2) Global chromatic aberration intensifies over time.
    // 3) Warping strength lerps to slight negative before to positive 1.
    // 4) A fade effect occurs as the camera zoom in and transitions to next scem
    class EncounterBattleTransition : public ITransitionEffect
    {
    private:
        enum class EBState { ZOOM_OUT, ZOOM_IN_FADE, COMPLETE };

        EBState m_state;
        float m_totalElapsed;      // Total elapsed time since the transition started.
        float m_stageElapsed;      // Elapsed time in the current stage.

        // Stage durations (in seconds)
        float m_zoomOutDuration;
        float m_zoomInDuration;    // Zoom in duration is also used for fade animation.

        // Fade parameters (fade anim runs concurrently with zoom-in).
        float m_initialAlpha;      // Starting opacity.
        float m_targetAlpha;       // Ending opacity.
        float m_currentAlpha;      // Current fade opacity.

        // Warp parameters
        float m_warpStart;
        float m_warpMid;
        float m_warpTarget;

        float m_originalWidth;
        float m_originalHeight;
        // For fade, a full-screen entity is used.
        FlexECS::Entity m_fadeEntity;

        // Chromatic aberration parameters.
        float m_chromaticStartIntensity;
        float m_chromaticTargetIntensity;

        // For glitch timing.
        std::chrono::high_resolution_clock::time_point m_startTime;

    public:
        // Constructs the EncounterBattleTransition.
        // totalDuration is split: e.g. 40% for zoom-out and 60% for zoom-in (with fade concurrent).
        EncounterBattleTransition(float totalDuration, float initialAlpha, float targetAlpha)
            : m_state(EBState::ZOOM_OUT),
            m_totalElapsed(0.0f),
            m_stageElapsed(0.0f),
            m_initialAlpha(initialAlpha),
            m_targetAlpha(targetAlpha),
            m_currentAlpha(initialAlpha),
            m_chromaticStartIntensity(0.0f),
            m_chromaticTargetIntensity(1.0f),
            m_warpStart(0.0f),
            m_warpMid(1.5f),
            m_warpTarget(-3.7f)
        {
            m_zoomOutDuration = totalDuration * 0.3f;
            m_zoomInDuration = totalDuration * 0.7f;  // Fade runs during this phase.
            m_originalWidth = CameraManager::GetMainGameCamera()->GetOrthoWidth();
            m_originalHeight = CameraManager::GetMainGameCamera()->GetOrthoHeight();
        }

        // Initializes the transition effect.
        void Start() override
        {
            m_state = EBState::ZOOM_OUT;
            m_totalElapsed = 0.0f;
            m_stageElapsed = 0.0f;
            m_startTime = std::chrono::high_resolution_clock::now();

            // Initialize chromatic aberration on all active post-processing entities.
            auto activeScene = FlexECS::Scene::GetActiveScene();
            if (activeScene)
            {
                for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
                {
                    auto transform = element.GetComponent<Transform>();
                    if (!transform || !transform->is_active)
                        continue;

                    auto marker = element.GetComponent<PostProcessingMarker>();
                    if (marker)
                    {
                        marker->enableChromaticAberration = true;
                        marker->enableWarp = true;
                    }

                    if (!element.HasComponent<PostProcessingChromaticAbberation>())
                        element.AddComponent<PostProcessingChromaticAbberation>({});
                    if (!element.HasComponent<PostProcessingWarp>())
                        element.AddComponent<PostProcessingWarp>({});

                    if (auto aberration = element.GetComponent<PostProcessingChromaticAbberation>())
                    {
                        aberration->redOffset.y = 0.0f;
                        aberration->greenOffset.y = 0.0f;
                        aberration->blueOffset.y = 0.0f;
                    }
                    if (auto warp = element.GetComponent<PostProcessingWarp>())
                    {
                        warp->warpStrength = m_warpStart;
                        warp->warpRadius = 1.0f;
                    }
                }
            }
            else
            {
                Log::Error("EncounterBattleTransition: Active scene not found during Start.");
            }

            // Stage 1: Initiate zoom-out.
            Application::MessagingSystem::Send("CameraZoomStart",
                std::pair<double, double>{ m_zoomOutDuration + m_zoomInDuration,
                                           Application::GetCurrentWindow()->GetWidth() * 0.5 });
            Log::Info("EncounterBattleTransition: Started zoom-out stage.");
        }

        // Updates the transition effect over time.
        void Update(float dt) override
        {
            if (m_state == EBState::COMPLETE)
                return;

            m_totalElapsed += dt;
            m_stageElapsed += dt;

            // Update global chromatic aberration concurrently.
            #pragma region Chromatic Aberration
            float totalDuration = m_zoomOutDuration + m_zoomInDuration;
            float chromaticProgress = m_totalElapsed / totalDuration;
            float currentIntensity = FlexMath::Lerp(m_chromaticStartIntensity, m_chromaticTargetIntensity, chromaticProgress);

            auto activeScene = FlexECS::Scene::GetActiveScene();
            if (activeScene)
            {
                // Use m_startTime for glitch timing.
                auto now = std::chrono::high_resolution_clock::now();
                for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
                {
                    auto transform = element.GetComponent<Transform>();
                    if (!transform || !transform->is_active)
                        continue;
                    if (auto aberration = element.GetComponent<PostProcessingChromaticAbberation>())
                    {
                        aberration->intensity = currentIntensity;
                        // Glitch effect: randomized offset per frame.
                        float amount = (float)(rand() % (int)FlexMath::Lerp(1.0f, 100.0f, chromaticProgress));
                        aberration->redOffset.x = amount;
                        aberration->greenOffset.x = 0.0f;
                        aberration->blueOffset.x = -amount;
                    }
                }
            }
            #pragma endregion

            // Process states.
            switch (m_state)
            {
            case EBState::ZOOM_OUT:
            {
                // Initiate warp to negative.
                if (activeScene)
                {
                    for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
                    {
                        auto transform = element.GetComponent<Transform>();
                        if (!transform || !transform->is_active)
                            continue;

                        if (auto warp = element.GetComponent<PostProcessingWarp>())
                            warp->warpStrength = FlexMath::Lerp(m_warpStart, m_warpMid, m_stageElapsed / m_zoomOutDuration);
                    }
                }

                // Triggers once dont worry
                if (m_stageElapsed >= m_zoomOutDuration)
                {
                    m_state = EBState::ZOOM_IN_FADE;
                    m_stageElapsed = 0.0f;

                    // Create fade entity at the start of zoom-in.
                    if (activeScene)
                    {
                        m_fadeEntity = activeScene->CreateEntity("EncounterFadeFrame");
                        m_fadeEntity.AddComponent<Position>({});
                        m_fadeEntity.AddComponent<Rotation>({});
                        m_fadeEntity.AddComponent<Scale>({ Vector3(9999, 9999, 0) });
                        m_fadeEntity.AddComponent<Transform>({});
                        m_fadeEntity.AddComponent<ZIndex>({ 9999 });
                        m_fadeEntity.AddComponent<Sprite>({});
                        if (auto sprite = m_fadeEntity.GetComponent<Sprite>()) {
                            sprite->opacity = 0.f;
                        }
                        else 
                        {
                            Log::Warning("EncounterBattleTransition: Sprite component not found on fade entity.");
                        }
                    }
                    Log::Info("EncounterBattleTransition: Transitioning to zoom-in (with fade) stage.");
                }
                break;
            }
            case EBState::ZOOM_IN_FADE:
            {
                // Fade effect runs concurrently with zoom-in & warp.
                if (activeScene)
                {
                    for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
                    {
                        auto transform = element.GetComponent<Transform>();
                        if (!transform || !transform->is_active)
                            continue;

                        if (auto warp = element.GetComponent<PostProcessingWarp>())
                            warp->warpStrength = FlexMath::Lerp(m_warpMid, m_warpTarget, m_stageElapsed/ m_zoomInDuration);
                    }
                }
                if (m_stageElapsed >= (m_zoomInDuration / 2.0f)) 
                {
                    float fadeProgress = (m_stageElapsed - (m_zoomInDuration / 2.0f)) / (m_zoomInDuration / 2.0f);
                    if (fadeProgress > 1.0f)
                        fadeProgress = 1.0f; 
                    m_currentAlpha = FlexMath::Lerp(m_initialAlpha, m_targetAlpha, fadeProgress);
                }
                if (m_fadeEntity)
                {
                    if (auto sprite = m_fadeEntity.GetComponent<Sprite>())
                        sprite->opacity = m_currentAlpha;
                    else
                        Log::Warning("EncounterBattleTransition: Sprite missing during fade update.");
                }
                if (m_stageElapsed >= m_zoomInDuration)
                {
                    m_state = EBState::COMPLETE;
                    Log::Info("EncounterBattleTransition: Zoom-in and fade stage complete.");
                    Application::MessagingSystem::Send("CameraZoomStart",
                                           std::pair<double, double>{0.0001,
                                            m_originalWidth});
                }
                break;
            }
            case EBState::COMPLETE:
            default:
                break;
            }
        }

        // Returns whether the encounter transition is complete.
        bool IsComplete() const override
        {
            return m_state == EBState::COMPLETE;
        }

        // Returns the transition type.
        // For encounter transitions we can choose a custom type.
        TransitionType GetType() const override
        {
            // Here we choose ENCOUNTER_TRANSITION_BATTLE_IN to indicate this special effect.
            return TransitionType::ENCOUNTER_TRANSITION_BATTLE_IN;
        }

        // Stops the transition and cleans up the fade entity.
        void Stop() override
        {
            auto activeScene = FlexECS::Scene::GetActiveScene();
            if (activeScene && m_fadeEntity)
            {
                m_fadeEntity = FlexECS::Entity();
            }
        }
    };

    #pragma endregion
    
    #pragma endregion

    // TransitionHandlerScript manages queued transition effects and processes them.
    class TransitionHandlerScript : public IScript
    {
        std::unique_ptr<ITransitionEffect> m_currentTransition;  ///< Currently active transition effect.
        std::queue<std::unique_ptr<ITransitionEffect>> m_transitionQueue; ///< Queue of pending transition effects.
        bool m_completionNotified;  ///< Ensures only one notification is sent per transition.
    public:
        // Constructs the TransitionHandlerScript and registers it with the script registry.
        TransitionHandlerScript() : m_completionNotified(false)
        {
            ScriptRegistry::RegisterScript(this);
        }

        // Returns the name of this script.
        std::string GetName() const override { return "TransitionHandlerScript"; }

        // Awake is called when the script instance is created.
        void Awake() override
        {
            // No initialization required at Awake.
        }

        // Start is called before the first update.
        void Start() override
        {
            // Additional initialization if needed.
        }

        // Update processes incoming transition commands and updates active transitions.
        void Update() override
        {
            // Listen for transition commands (using an int and double pair).
            std::pair<int, double> transitionStartCommand = Application::MessagingSystem::Receive<std::pair<int, double>>("TransitionStart");

            if (transitionStartCommand.first != 0 && transitionStartCommand.second > 0.0)
            {
                TransitionType requestedType = static_cast<TransitionType>(transitionStartCommand.first);
                float duration = static_cast<float>(transitionStartCommand.second);

                // Create the appropriate transition effect based on the requested type.
                std::unique_ptr<ITransitionEffect> newTransition;
                if (requestedType == TransitionType::FADE_IN)
                {
                    newTransition = std::make_unique<FadeTransition>(duration, 1.0f, 0.0f);
                }
                else if (requestedType == TransitionType::FADE_OUT)
                {
                    newTransition = std::make_unique<FadeTransition>(duration, 0.0f, 1.0f);
                }
                else if (requestedType == TransitionType::ENCOUNTER_TRANSITION_BATTLE_IN)
                {
                    newTransition = std::make_unique<EncounterBattleTransition>(duration, 0.0f, 1.0f);
                }
                else
                {
                    Log::Warning("TransitionHandlerScript: Invalid transition type received.");
                }
                if (newTransition)
                {
                    m_transitionQueue.push(std::move(newTransition));
                    Log::Info("TransitionHandlerScript: Queued transition " + std::to_string(static_cast<int>(requestedType)) +
                              " with duration " + std::to_string(duration));
                }
            }

            // If no transition is active, start the next one from the queue.
            if (!m_currentTransition && !m_transitionQueue.empty())
            {
                m_currentTransition = std::move(m_transitionQueue.front());
                m_transitionQueue.pop();
                m_currentTransition->Start();
                m_completionNotified = false;
            }

            // Update the active transition effect.
            if (m_currentTransition)
            {
                float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
                m_currentTransition->Update(dt);

                // Once the current transition completes, send a notification once.
                if (m_currentTransition->IsComplete())
                {
                    if (!m_completionNotified)
                    {
                        TransitionType completedType = m_currentTransition->GetType();
                        Application::MessagingSystem::Send("TransitionCompleted", static_cast<int>(completedType));
                        m_completionNotified = true;
                    }
                    // If a new transition is queued, stop the finished one and prepare for the next.
                    if (!m_transitionQueue.empty())
                    {
                        m_currentTransition->Stop();
                        m_currentTransition.reset();
                        m_completionNotified = false;
                    }
                }
            }
        }

        // Stops the current transition and clears all queued transitions.
        void Stop() override
        {
            if (m_currentTransition)
            {
                m_currentTransition->Stop();
                m_currentTransition.reset();
            }
            while (!m_transitionQueue.empty())
            {
                m_transitionQueue.front()->Stop();
                m_transitionQueue.pop();
            }
        }
    };

    // Static instance ensures the TransitionHandlerScript is registered at startup.
    static TransitionHandlerScript transitionHandlerScript;

} // namespace Game
