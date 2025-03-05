#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
using namespace FlexEngine;

namespace Game
{
    /*!************************************************************************
    * \class TransitionHandlerScript
    * \brief This script handles transition effects (e.g., fade) between scenes.
    *
    * \usage Attach this script to an always-active object or system. The script
    * listens for a message ("DoTransitionFade") to start a transition, creates a
    * temporary transition entity, performs a fade effect by lerping the sprite's
    * opacity, and then sends a message ("TransitionCompleted") when finished.
    *************************************************************************/
    class TransitionHandlerScript : public IScript
    {
    private:
        // Transition entity used for displaying the effect.
        FlexECS::Entity m_transitionFrame;

        // Timing and effect parameters.
        float m_elapsedTime = 0.0f;           // Time elapsed since transition start.
        float m_transitionDuration = 1.0f;    // Total duration of the transition.
        float m_initialAlpha = 0.0f;          // Starting opacity.
        float m_targetAlpha = 1.0f;           // Ending opacity.
        float m_currentAlpha = 0.0f;          // Current computed opacity.
        bool m_isTransitionActive = false;    // Flag to indicate an active transition.

        // Simple linear interpolation helper.
        float Lerp(float start, float end, float t)
        {
            return start + (end - start) * t;
        }

    public:
        TransitionHandlerScript()
        {
            ScriptRegistry::RegisterScript(this);
        }

        std::string GetName() const override { return "TransitionHandlerScript"; }

        void Awake() override
        {
            // Optionally, initialize variables or subscribe to messages here.
        }

        void Start() override
        {
            // Initialization logic, if needed.
            // For example, subscribe to messaging events.
        }

        void Update() override
        {
            // Listen for a message to start a fade transition.
            bool startTransitionMsg = Application::MessagingSystem::Receive<bool>("DoTransitionFade");
            if (startTransitionMsg && !m_isTransitionActive)
            {
                // Start a fade transition with desired parameters.
                // (e.g., 1 second duration, fading from opacity 0 to 1).
                StartTransition(1.0f, 0.0f, 1.0f);
            }

            // If a transition is active, update its progress.
            if (m_isTransitionActive)
            {
                float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
                m_elapsedTime += dt;
                float t = m_elapsedTime / m_transitionDuration;
                if (t > 1.0f)
                    t = 1.0f;

                m_currentAlpha = Lerp(m_initialAlpha, m_targetAlpha, t);

                // Update the sprite component's opacity.
                auto sprite = m_transitionFrame.GetComponent<Sprite>();
                if (sprite)
                    sprite->opacity = m_currentAlpha;

                // If the transition has completed...
                if (m_elapsedTime >= m_transitionDuration)
                {
                    m_isTransitionActive = false;

                    // Destroy the temporary transition entity.
                    auto currentScene = FlexECS::Scene::GetActiveScene();
                    if (currentScene)
                        currentScene->DestroyEntity(m_transitionFrame);
                    m_transitionFrame = FlexECS::Entity(); // Reset the handle.

                    // Send a message indicating the transition is complete.
                    Application::MessagingSystem::Send("TransitionCompleted", true);
                }
            }
        }

        void Stop() override
        {
            // Cleanup any ongoing transition effects.
            m_isTransitionActive = false;
            m_elapsedTime = 0.0f;
            auto sprite = m_transitionFrame.GetComponent<Sprite>();
            if (sprite)
                sprite->opacity = m_initialAlpha;
        }

        // Initiates the transition with the specified parameters.
        void StartTransition(float duration, float initialAlpha, float targetAlpha)
        {
            m_transitionDuration = duration;
            m_initialAlpha = initialAlpha;
            m_targetAlpha = targetAlpha;
            m_elapsedTime = 0.0f;
            m_isTransitionActive = true;
            m_currentAlpha = m_initialAlpha;

            // Create the transition entity in the active scene.
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

                // Set the initial opacity on the sprite.
                auto sprite = m_transitionFrame.GetComponent<Sprite>();
                if (sprite)
                    sprite->opacity = m_currentAlpha;
            }
        }
    };

    // Static instance to ensure the script is registered.
    static TransitionHandlerScript transitionScript;
}