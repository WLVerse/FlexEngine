#include "CameraSystemLayer.h"
#include <cstdlib>    // For std::rand()
#include <ctime>      // For std::time()
#include <algorithm>  // For std::min and std::max

namespace Game 
{

    CameraSystemLayer::CameraSystemLayer() : Layer("Camera System Layer") 
    {
        // Seed the random generator for shake offsets.
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    #pragma region Helper Func
    // Generate a random shake offset based on the given intensity.
    Vector3 CameraSystemLayer::GenerateShakeOffset(float intensity)
    {
        float offsetX = ((std::rand() % 2000) / 1000.0f - 1.0f) * intensity;
        float offsetY = ((std::rand() % 2000) / 1000.0f - 1.0f) * intensity;
        return Vector3(offsetX, offsetY, 0.0f);
    }
    #pragma endregion

    void CameraSystemLayer::OnAttach()
    {
        m_zoomBase = Application::GetCurrentWindow()->GetWidth(); // REMEMBER THIS IS IMPORTANT IF ZOOM IS MESSED UP ON START
    }

    void CameraSystemLayer::OnDetach()
    {
        // No additional cleanup required.
    }

    void CameraSystemLayer::Update()
    {
        // Try to set up the main camera if not already done.
        if (!CameraManager::has_main_camera)
        {
            CameraManager::TryMainCamera();
        }

        // Get the entity of current main camera
        FlexECS::Entity m_mainCameraEntity;
        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Camera>())
        {
            if (elem.Get() == CameraManager::GetMainGameCameraID()) m_mainCameraEntity = elem;
        }

        float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // ----------------------------------------------------------------
        // Enqueue new camera effects from messages.
        // ----------------------------------------------------------------

        // --- SHAKE EFFECTS ---
        // Standard shake effect.
        // Message: "CameraShakeStart"
        // Parameters: (duration, intensity)
        if (auto shakeParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeStart");
            shakeParams.first > 0.0 && shakeParams.second > 0.0)
        {
            // Only capture the original camera position if no shake effect is active.
            if (m_shakeEffects.empty() && m_mainCameraEntity)
            {
                if (auto pos = m_mainCameraEntity.GetComponent<Position>())
                    m_originalCameraPos = pos->position;
            }
            m_shakeEffects.push_back({ static_cast<float>(shakeParams.first), 0.0f,
                                       static_cast<float>(shakeParams.second), false });
            Log::Info("Queued standard shake effect.");
        }

        // Lerp (ramped) shake effect.
        // Message: "CameraShakeLerpStart"
        // Parameters: (duration, intensity)
        if (auto shakeLerpParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeLerpStart");
            shakeLerpParams.first > 0.0 && shakeLerpParams.second > 0.0)
        {
            // Capture the baseline only if no shake effect is currently active.
            if (m_shakeEffects.empty() && m_mainCameraEntity)
            {
                if (auto pos = m_mainCameraEntity.GetComponent<Position>())
                    m_originalCameraPos = pos->position;
            }
            m_shakeEffects.push_back({ static_cast<float>(shakeLerpParams.first), 0.0f,
                                       static_cast<float>(shakeLerpParams.second), true });
            Log::Info("Queued lerp shake effect.");
        }

        // Persistent zoom effect. *DISABLED, BUGGY*
        // Message: "CameraZoomStart"
        // Parameters:
        //   - Parameter 1 (double duration): Duration (in seconds) over which the zoom occurs.
        //   - Parameter 2 (double targetOrthoWidth): The target orthographic width to achieve by the end of the effect.
        // A new zoom message replaces any existing zoom effect.
        //if (auto zoomParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraZoomStart");
        //    zoomParams.first > 0.0)
        //{
        //    // Use current camera ortho width as the starting point.
        //    float currentOrthoWidth = m_zoomBase;
        //    if (m_mainCameraEntity) 
        //    {
        //       currentOrthoWidth = m_mainCameraEntity.GetComponent<Camera>()->GetOrthoWidth();
        //    }
        //    m_zoomEffect = ZoomEffect{
        //        static_cast<float>(zoomParams.first), // duration for persistent zoom
        //        0.0f,
        //        static_cast<float>(zoomParams.second), // target ortho width
        //        currentOrthoWidth,                     // starting from current camera width
        //        false,                                 // not auto-return
        //        0.0f, 0.0f, 0.0f                       // auto-return parameters not used
        //    };
        //    Log::Info("Queued persistent zoom effect.");
        //}

        // Auto-return zoom effect.
        // Message: "CameraZoomAutoReturnStart"
        // Parameters: (lerpDuration, holdDuration, targetOrthoWidth)
        if (auto zoomAutoParams = Application::MessagingSystem::Receive<std::tuple<double, double, double>>("CameraZoomAutoReturnStart");
            std::get<0>(zoomAutoParams) > 0.0)
        {
            float lerpDuration = static_cast<float>(std::get<0>(zoomAutoParams));
            float holdDuration = static_cast<float>(std::get<1>(zoomAutoParams));
            float targetOrthoWidth = static_cast<float>(std::get<2>(zoomAutoParams));
            float currentOrthoWidth = targetOrthoWidth; // fallback value

            // Capture the zoom baseline if there is no active zoom effect.
            if (!m_zoomActive && m_mainCameraEntity)
            {
                if (auto cam = m_mainCameraEntity.GetComponent<Camera>())
                {
                    currentOrthoWidth = cam->GetOrthoWidth();
                    m_zoomBase = currentOrthoWidth;
                }
            }
            else if (m_zoomActive && m_mainCameraEntity)
            {
                // Optionally update the baseline if a new zoom message is received.
                if (auto cam = m_mainCameraEntity.GetComponent<Camera>())
                {
                    currentOrthoWidth = cam->GetOrthoWidth();
                    m_zoomBase = currentOrthoWidth;
                }
            }

            m_zoomEffect = ZoomEffect{
                0.0f,             // elapsed
                0.0f,             // duration not used here
                targetOrthoWidth, // target ortho width
                currentOrthoWidth,// initial ortho width captured at message time
                true,             // auto-return enabled
                lerpDuration,
                holdDuration,
                2 * lerpDuration + holdDuration // total duration: ramp up + hold + ramp down
            };
            m_zoomActive = true;
            Log::Info("Queued auto-return zoom effect.");
        }


        // ----------------------------------------------------------------
                // Process active shake effects.
                // ----------------------------------------------------------------
        Vector3 cumulativeShake(0.0f);
        for (int i = static_cast<int>(m_shakeEffects.size()) - 1; i >= 0; --i)
        {
            auto& effect = m_shakeEffects[i];
            effect.elapsed += deltaTime;
            float intensity = effect.intensity;
            if (effect.lerp)
            {
                float progress = std::min(effect.elapsed / effect.duration, 1.0f);
                float factor = (progress < 0.5f) ? (progress * 2.0f) : ((1.0f - progress) * 2.0f);
                intensity *= factor;
            }
            cumulativeShake += GenerateShakeOffset(intensity);

            if (effect.elapsed >= effect.duration)
            {
                m_shakeEffects.erase(m_shakeEffects.begin() + i);
                Log::Info("Shake effect completed.");
                Application::MessagingSystem::Send("CameraShakeCompleted", 1);
            }
        }

        // ----------------------------------------------------------------
        // Process active zoom effects.
        // ----------------------------------------------------------------
        float deltaZoom = 0.0f;
        if (m_zoomActive)
        {
            m_zoomEffect.elapsed += deltaTime;
            if (m_zoomEffect.autoReturn)
            {
                if (m_zoomEffect.elapsed < m_zoomEffect.lerpDuration)
                {
                    // Ramp up phase: interpolate from baseline to target.
                    float t = m_zoomEffect.elapsed / m_zoomEffect.lerpDuration;
                    deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * t;
                }
                else if (m_zoomEffect.elapsed < m_zoomEffect.lerpDuration + m_zoomEffect.holdDuration)
                {
                    // Hold phase at target.
                    deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth);
                }
                else if (m_zoomEffect.elapsed < m_zoomEffect.totalDuration)
                {
                    // Ramp down phase: interpolate back toward the baseline.
                    float t = (m_zoomEffect.elapsed - (m_zoomEffect.lerpDuration + m_zoomEffect.holdDuration)) / m_zoomEffect.lerpDuration;
                    deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * (1.0f - t);
                }
                else
                {
                    // Zoom effect completed.
                    deltaZoom = 0.0f;
                    Log::Info("Zoom effect completed.");
                    Application::MessagingSystem::Send("CameraZoomCompleted", 1);
                    m_zoomActive = false;
                }
            }
            else
            {
                m_zoomEffect.elapsed += deltaTime;
                float progress = std::min(m_zoomEffect.elapsed / m_zoomEffect.duration, 1.0f);
                deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * progress;
                if (m_zoomEffect.elapsed >= m_zoomEffect.duration)
                {
                    Log::Info("Zoom effect completed.");
                    Application::MessagingSystem::Send("CameraZoomCompleted", 1);
                    m_zoomActive = false;
                }
            }
        }

        // Calculate the effective orthographic dimensions.
        float effectiveOrthoWidth = std::max(m_zoomBase + deltaZoom, m_minOrthoWidth);
        float effectiveOrthoHeight = effectiveOrthoWidth / m_baseAspectRatio;

        // ----------------------------------------------------------------
        // Apply effects to the main camera.
        // ----------------------------------------------------------------
        if (m_mainCameraEntity)
        {
            // Apply shake effects to the main camera.
            if (!m_shakeEffects.empty())
            {
                if (auto pos = m_mainCameraEntity.GetComponent<Position>())
                    pos->position = m_originalCameraPos + cumulativeShake;
                else
                    Log::Warning("Main camera: Position component missing.");
            }

            // Apply zoom effects to the main camera.
            if (auto cam = m_mainCameraEntity.GetComponent<Camera>())
            {
                cam->SetOrthographic(-effectiveOrthoWidth / 2, effectiveOrthoWidth / 2,
                                     -effectiveOrthoHeight / 2, effectiveOrthoHeight / 2);
                cam->Update();
            }
            else
            {
                Log::Warning("Main camera: Camera component missing.");
            }
        }

        // ----------------------------------------------------------------
        // Update all cameras in the scene.
        // ----------------------------------------------------------------
        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Camera>())
        {
            elem.GetComponent<Camera>()->SetViewMatrix(elem.GetComponent<Position>()->position);
            elem.GetComponent<Camera>()->Update();
        }
    }

} // namespace Game
