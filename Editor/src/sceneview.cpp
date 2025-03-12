#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
#include <cstdlib>   // For std::rand()
#include <ctime>     // For std::time()
#include <algorithm> // For std::max

using namespace FlexEngine;

namespace FlexEngine
{
    /*!
    ************************************************************************
    * \class CameraHandler
    * \brief Handles additional camera functionalities such as camera shake
    * and zoom. This script listens for messages to start an effect and sends a
    * notification when the effect completes.
    *
    * \usage Place this script on an object that should manage camera effects.
    *************************************************************************/
    class CameraHandler : public IScript
    {
    private:
        // Handle to the main camera entity.
        FlexECS::Entity m_mainCameraEntity;

        // Variables for Camera Shake effect.
        bool m_shakeActive = false;
        float m_shakeDuration = 0.0f;
        float m_shakeElapsed = 0.0f;
        float m_shakeIntensity = 0.0f;
        Vector3 m_originalCameraPos;

        // Variables for Camera Zoom effect.
        bool m_zoomActive = false;
        float m_zoomDuration = 0.0f;
        float m_zoomElapsed = 0.0f;
        float m_zoomTarget = 0.0f;   // Target orthographic width.
        float m_zoomInitial = 0.0f;  // Starting orthographic width.

        // Constants for orthographic projection.
        const float m_baseAspectRatio = 16.0f / 9.0f; // Adjust as needed.
        const float m_minZoom = 10.0f;                  // Minimum orthographic width.

    public:
        CameraHandler()
        {
            // Register the script with the system.
            ScriptRegistry::RegisterScript(this);
            // Seed random generator for shake offsets.
            std::srand(static_cast<unsigned>(std::time(nullptr)));
        }

        std::string GetName() const override { return "CameraHandler"; }

        void Awake() override {}

        void Start() override
        {
            // Retrieve the main camera entity from the active scene.
            auto scene = FlexECS::Scene::GetActiveScene();
            if (scene)
            {
                for (auto& elem : scene->CachedQuery<Position, Transform, Camera>())
                {
                    if (elem.Get() == CameraManager::GetMainGameCameraID())
                    {
                        m_mainCameraEntity = elem;
                        break;
                    }
                }
            }
            else
            {
                Log::Error("CameraHandler: Active scene not found.");
            }

            // Cache the original camera position and orthographic width.
            if (m_mainCameraEntity)
            {
                auto pos = m_mainCameraEntity.GetComponent<Position>();
                if (pos)
                {
                    m_originalCameraPos = pos->position;
                }
                else
                {
                    Log::Warning("CameraHandler: Position component missing on main camera.");
                }

                auto cam = m_mainCameraEntity.GetComponent<Camera>();
                if (cam)
                {
                    // Assume GetOrthoWidth returns the current orthographic width.
                    m_zoomInitial = cam->GetOrthoWidth();
                }
                else
                {
                    Log::Warning("CameraHandler: Camera component missing on main camera.");
                }
            }
        }

        void Update() override
        {
            // Get the frame delta time.
            float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

            // -------------------------
            // Check for Shake Message:
            // Expected message: "CameraShakeStart" with a pair (duration, intensity).
            // -------------------------
            auto shakeMsg = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeStart");
            if (shakeMsg.first > 0.0 && shakeMsg.second > 0.0)
            {
                m_shakeDuration = static_cast<float>(shakeMsg.first);
                m_shakeIntensity = static_cast<float>(shakeMsg.second);
                m_shakeElapsed = 0.0f;
                m_shakeActive = true;
                Log::Info("CameraHandler: Starting shake effect (duration: " +
                          std::to_string(m_shakeDuration) + ", intensity: " +
                          std::to_string(m_shakeIntensity) + ").");
            }

            // -------------------------
            // Check for Zoom Message:
            // Expected message: "CameraZoomStart" with a pair (duration, targetOrthoWidth).
            // -------------------------
            auto zoomMsg = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraZoomStart");
            if (zoomMsg.first > 0.0)
            {
                m_zoomDuration = static_cast<float>(zoomMsg.first);
                m_zoomTarget = static_cast<float>(zoomMsg.second);
                m_zoomElapsed = 0.0f;
                m_zoomActive = true;
                // Capture the current orthographic width as the starting value.
                auto cam = m_mainCameraEntity.GetComponent<Camera>();
                if (cam)
                {
                    m_zoomInitial = cam->GetOrthoWidth();
                }
                Log::Info("CameraHandler: Starting zoom effect (duration: " +
                          std::to_string(m_zoomDuration) + ", target ortho width: " +
                          std::to_string(m_zoomTarget) + ").");
            }

            // -------------------------
            // Process Camera Shake Effect:
            // -------------------------
            if (m_shakeActive && m_mainCameraEntity)
            {
                m_shakeElapsed += deltaTime;
                // Generate a random offset within [-intensity, intensity] for X and Y.
                float offsetX = ((std::rand() % 2000) / 1000.0f - 1.0f) * m_shakeIntensity;
                float offsetY = ((std::rand() % 2000) / 1000.0f - 1.0f) * m_shakeIntensity;

                auto pos = m_mainCameraEntity.GetComponent<Position>();
                if (pos)
                {
                    pos->position = m_originalCameraPos + Vector3(offsetX, offsetY, 0.0f);
                }
                else
                {
                    Log::Warning("CameraHandler: Position component missing during shake update.");
                }

                // If the shake duration has elapsed, reset the camera and notify.
                if (m_shakeElapsed >= m_shakeDuration)
                {
                    if (pos)
                    {
                        pos->position = m_originalCameraPos;
                    }
                    m_shakeActive = false;
                    Log::Info("CameraHandler: Shake effect completed.");
                    Application::MessagingSystem::Send("CameraShakeCompleted", 1);
                }
            }

            // -------------------------
            // Process Camera Zoom Effect:
            // Updated to use orthographic projection parameters similar to your working code.
            // -------------------------
            if (m_zoomActive && m_mainCameraEntity)
            {
                m_zoomElapsed += deltaTime;
                float t = m_zoomElapsed / m_zoomDuration;
                if (t > 1.0f)
                    t = 1.0f;

                // Interpolate between the initial and target orthographic width.
                float currentOrthoWidth = m_zoomInitial + (m_zoomTarget - m_zoomInitial) * t;
                // Ensure the orthographic width doesn't go below the minimum.
                currentOrthoWidth = std::max(currentOrthoWidth, m_minZoom);
                // Calculate the corresponding height based on the aspect ratio.
                float currentOrthoHeight = currentOrthoWidth / m_baseAspectRatio;

                auto cam = m_mainCameraEntity.GetComponent<Camera>();
                if (cam)
                {
                    cam->SetOrthographic(-currentOrthoWidth / 2, currentOrthoWidth / 2,
                                           -currentOrthoHeight / 2, currentOrthoHeight / 2);
                    cam->Update();
                }
                else
                {
                    Log::Warning("CameraHandler: Camera component missing during zoom update.");
                }

                // When the zoom effect completes, send a completion message.
                if (m_zoomElapsed >= m_zoomDuration)
                {
                    m_zoomActive = false;
                    Log::Info("CameraHandler: Zoom effect completed.");
                    Application::MessagingSystem::Send("CameraZoomCompleted", 1);
                }
            }
        }

        void Stop() override
        {
            // On stopping, reset camera position and orthographic projection to their original states.
            if (m_mainCameraEntity)
            {
                auto pos = m_mainCameraEntity.GetComponent<Position>();
                if (pos)
                {
                    pos->position = m_originalCameraPos;
                }
                auto cam = m_mainCameraEntity.GetComponent<Camera>();
                if (cam)
                {
                    // Reset to initial orthographic parameters.
                    float initialOrthoHeight = m_zoomInitial / m_baseAspectRatio;
                    cam->SetOrthographic(-m_zoomInitial / 2, m_zoomInitial / 2,
                                         -initialOrthoHeight / 2, initialOrthoHeight / 2);
                    cam->Update();
                }
            }
            m_shakeActive = false;
            m_zoomActive = false;
            Log::Info("CameraHandler: Effects stopped and camera reset.");
        }
    };

    // Static instance to ensure the script is registered.
    static CameraHandler CamHandler;
}
