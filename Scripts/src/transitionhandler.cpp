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

    #pragma endregion

    // TransitionHandlerScript manages queued transition effects and processes them.
    class TransitionHandlerScript : public IScript
    {
        std::unique_ptr<ITransitionEffect> m_currentTransition;  ///< Currently active transition effect.
        std::queue<std::unique_ptr<ITransitionEffect>> m_transitionQueue; ///< Queue of pending transition effects.
        bool m_completionNotified;  ///< Ensures only one notification is sent per transition.
    public:
        // Constructs the TransitionHandlerScript and registers it with the script registry.
        TransitionHandlerScript()
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
