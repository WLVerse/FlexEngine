#include "camerasystemlayer.h"
#include <cstdlib>    // For std::rand()
#include <ctime>      // For std::time()
#include <algorithm>  // For std::max

namespace Game
{
    CameraSystemLayer::CameraSystemLayer() : Layer("Camera System Layer")
    {
        // Seed the random generator for shake offsets.
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    void CameraSystemLayer::OnAttach()
    {
        // Retrieve the main camera entity from the active scene.
        auto scene = FlexECS::Scene::GetActiveScene();
        if (scene)
        {
            for (auto& elem : scene->CachedQuery<Position, Camera>())
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
            Log::Error("CameraSystemLayer: Active scene not found.");
        }

        // Cache the original camera position and initial orthographic width.
        if (m_mainCameraEntity)
        {
            auto pos = m_mainCameraEntity.GetComponent<Position>();
            if (pos)
            {
                m_originalCameraPos = pos->position;
            }
            else
            {
                Log::Warning("CameraSystemLayer: Main camera Position component missing.");
            }

            auto cam = m_mainCameraEntity.GetComponent<Camera>();
            if (cam)
            {
                // Retrieve the current orthographic width.
                m_zoomInitial = cam->GetOrthoWidth();
            }
            else
            {
                Log::Warning("CameraSystemLayer: Main camera Camera component missing.");
            }
        }
    }

    void CameraSystemLayer::OnDetach()
    {
        // Cleanup if necessary.
    }

    void CameraSystemLayer::Update()
    {
        // Ensure the main camera exists.
        if (!CameraManager::has_main_camera)
        {
            CameraManager::TryMainCamera();
        }
        // If the main camera hasn't been set yet, try to find it.
        if (!m_mainCameraEntity)
        {
            auto scene = FlexECS::Scene::GetActiveScene();
            if (scene)
            {
                for (auto& elem : scene->CachedQuery<Position, Camera>())
                {
                    if (elem.Get() == CameraManager::GetMainGameCameraID())
                    {
                        m_mainCameraEntity = elem;
                        // Cache original position and zoom.
                        auto pos = m_mainCameraEntity.GetComponent<Position>();
                        if (pos)
                            m_originalCameraPos = pos->position;
                        auto cam = m_mainCameraEntity.GetComponent<Camera>();
                        if (cam)
                            m_zoomInitial = cam->GetOrthoWidth();
                        break;
                    }
                }
            }
        }

        // Get the delta time for this frame.
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
            Log::Info("CameraSystemLayer: Starting shake effect (duration: " +
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
            if (m_mainCameraEntity)
            {
                auto cam = m_mainCameraEntity.GetComponent<Camera>();
                if (cam)
                {
                    m_zoomInitial = cam->GetOrthoWidth();
                }
            }
            Log::Info("CameraSystemLayer: Starting zoom effect (duration: " +
                      std::to_string(m_zoomDuration) + ", target ortho width: " +
                      std::to_string(m_zoomTarget) + ").");
        }

        // -------------------------
        // Process Camera Shake Effect:
        // -------------------------
        if (m_shakeActive && m_mainCameraEntity)
        {
            m_shakeElapsed += deltaTime;
            // Generate random offsets in X and Y within [-intensity, intensity].
            float offsetX = ((std::rand() % 2000) / 1000.0f - 1.0f) * m_shakeIntensity;
            float offsetY = ((std::rand() % 2000) / 1000.0f - 1.0f) * m_shakeIntensity;

            auto pos = m_mainCameraEntity.GetComponent<Position>();
            if (pos)
            {
                pos->position = m_originalCameraPos + Vector3(offsetX, offsetY, 0.0f);
            }
            else
            {
                Log::Warning("CameraSystemLayer: Position component missing during shake update.");
            }

            // If the shake duration has elapsed, reset the camera position and send a completion message.
            if (m_shakeElapsed >= m_shakeDuration)
            {
                if (pos)
                {
                    pos->position = m_originalCameraPos;
                }
                m_shakeActive = false;
                Log::Info("CameraSystemLayer: Shake effect completed.");
                Application::MessagingSystem::Send("CameraShakeCompleted", 1);
            }
        }

        // -------------------------
        // Process Camera Zoom Effect:
        // -------------------------
        if (m_zoomActive && m_mainCameraEntity)
        {
            m_zoomElapsed += deltaTime;
            float t = m_zoomElapsed / m_zoomDuration;
            if (t > 1.0f)
                t = 1.0f;

            // Interpolate the orthographic width.
            float currentOrthoWidth = m_zoomInitial + (m_zoomTarget - m_zoomInitial) * t;
            // Ensure we do not go below the minimum width.
            currentOrthoWidth = std::max(currentOrthoWidth, m_minOrthoWidth);
            // Calculate the corresponding height based on the aspect ratio.
            float currentOrthoHeight = currentOrthoWidth / m_baseAspectRatio;

            auto cam = m_mainCameraEntity.GetComponent<Camera>();
            if (cam)
            {
                // Update the orthographic projection parameters.
                cam->SetOrthographic(-currentOrthoWidth / 2, currentOrthoWidth / 2,
                                       -currentOrthoHeight / 2, currentOrthoHeight / 2);
                cam->Update();
            }
            else
            {
                Log::Warning("CameraSystemLayer: Camera component missing during zoom update.");
            }

            // When the zoom effect completes, send a completion message.
            if (m_zoomElapsed >= m_zoomDuration)
            {
                m_zoomActive = false;
                Log::Info("CameraSystemLayer: Zoom effect completed.");
                Application::MessagingSystem::Send("CameraZoomCompleted", 1);
            }
        }

        // -------------------------
        // Update all cameras.
        // -------------------------
        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Camera>())
        {
            auto pos = elem.GetComponent<Position>();
            auto cam = elem.GetComponent<Camera>();
            if (pos && cam)
            {
                cam->SetViewMatrix(pos->position);
                cam->Update();
            }
        }
    }
}
