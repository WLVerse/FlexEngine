#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
#include <memory>
#include <queue>

using namespace FlexEngine;

namespace Game 
{
    #pragma region Transitions
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
        virtual void Start() = 0;
        virtual void Update(float dt) = 0;
        virtual bool IsComplete() const = 0;
        virtual TransitionType GetType() const = 0;
        virtual void Stop() = 0;
    };

    #pragma region Fade
    // FadeTransition implements a fade effect using easing.
    class FadeTransition : public ITransitionEffect 
    {
    private:
        FlexECS::Entity m_transitionFrame;
        float m_elapsedTime;
        float m_duration;
        float m_initialAlpha;
        float m_targetAlpha;
        float m_currentAlpha;
        bool m_complete;

    public:
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

        void Start() override 
        {
            // Additional initialization can be done here.
        }

        void Update(float dt) override 
        {
            if (m_complete)
                return;

            m_elapsedTime += dt;
            float progress = m_elapsedTime / m_duration;
            if (progress > 1.0f)
                progress = 1.0f;

            // Use the easing function (FlexMath::Lerp) to compute the opacity.
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

        bool IsComplete() const override 
        {
            return m_complete;
        }

        TransitionType GetType() const override 
        {
            return (m_targetAlpha > m_initialAlpha) ? TransitionType::FADE_IN : TransitionType::FADE_OUT;
        }

        void Stop() override 
        {
            auto currentScene = FlexECS::Scene::GetActiveScene();
            if (currentScene && m_transitionFrame) 
            {
                currentScene->DestroyEntity(m_transitionFrame);
                m_transitionFrame = FlexECS::Entity(); // Reset handle.
            }
            else 
            {
                Log::Warning("FadeTransition: Unable to stop transition; entity or scene missing.");
            }
        }
    };
    #pragma endregion

    #pragma endregion


    class TransitionHandlerScript : public IScript 
    {
    private:
        std::unique_ptr<ITransitionEffect> m_currentTransition;
        std::queue<std::unique_ptr<ITransitionEffect>> m_transitionQueue; // Queue for stacking transitions.

    public:
        TransitionHandlerScript() 
        {
            ScriptRegistry::RegisterScript(this);
        }

        std::string GetName() const override { return "TransitionHandlerScript"; }

        void Awake() override 
        {
        }

        void Start() override 
        {
            // Initialization if needed.
        }

        void Update() override 
        {
            // Listen for transition messages using int identifiers.
            int transitionStartCommand = Application::MessagingSystem::Receive<int>("TransitionStart");

            // Helper lambda to enqueue a new transition.
            auto queueTransition = [this](TransitionType type) 
            {
                if (type == TransitionType::FADE_IN) 
                {
                    auto transition = std::make_unique<FadeTransition>(1.0f, 0.0f, 1.0f);
                    m_transitionQueue.push(std::move(transition));
                }
                else if (type == TransitionType::FADE_OUT)
                {
                    auto transition = std::make_unique<FadeTransition>(1.0f, 1.0f, 0.0f);
                    m_transitionQueue.push(std::move(transition));
                }
                Log::Info("TransitionHandlerScript: Queued transition " + std::to_string(static_cast<int>(type)));
            };

            // Enqueue transitions if messages were received.
            if (transitionStartCommand != 0)
            {
                queueTransition(static_cast<TransitionType>(transitionStartCommand));
            }

            // If no transition is currently running, start the next one from the queue.
            if (!m_currentTransition && !m_transitionQueue.empty()) 
            {
                m_currentTransition = std::move(m_transitionQueue.front());
                m_transitionQueue.pop();
                m_currentTransition->Start();
            }

            // Update the current transition.
            if (m_currentTransition) 
            {
                float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
                m_currentTransition->Update(dt);

                // When complete, notify listeners and clear the current transition.
                if (m_currentTransition->IsComplete()) 
                {
                    TransitionType completedType = m_currentTransition->GetType();
                    Application::MessagingSystem::Send("TransitionCompleted", static_cast<int>(completedType));
                    m_currentTransition->Stop();
                    m_currentTransition.reset();
                }
            }
        }

        void Stop() override 
        {
            if (m_currentTransition) 
            {
                m_currentTransition->Stop();
                m_currentTransition.reset();
            }
            // Clear any queued transitions.
            while (!m_transitionQueue.empty()) 
            {
                m_transitionQueue.front()->Stop();
                m_transitionQueue.pop();
            }
        }
    };

    // Static instance to ensure the script is registered.
    static TransitionHandlerScript transitionHandlerScript;

} // namespace Game
