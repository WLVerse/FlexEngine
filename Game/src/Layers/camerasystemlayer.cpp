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
    // Ensure the main camera is set by querying the active scene.
    void CameraSystemLayer::EnsureMainCamera()
    {
        if (m_mainCameraEntity)
            return;

        auto scene = FlexECS::Scene::GetActiveScene();
        if (!scene)
        {
            Log::Error("CameraSystemLayer: No active scene.");
            return;
        }
        for (auto& elem : scene->CachedQuery<Position, Camera>())
        {
            if (elem.Get() == CameraManager::GetMainGameCameraID())
            {
                m_mainCameraEntity = elem;
                if (auto pos = m_mainCameraEntity.GetComponent<Position>())
                    m_originalCameraPos = pos->position;
                if (auto cam = m_mainCameraEntity.GetComponent<Camera>())
                    m_zoomBase = cam->GetOrthoWidth();
                break;
            }
        }
    }

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
        EnsureMainCamera();

        float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // ----------------------------------------------------------------
        // Enqueue new camera effects from messages.
        // ----------------------------------------------------------------

        // Standard shake effect.
        // Message: "CameraShakeStart"
        // Parameters:
        //   - Parameter 1 (double duration): Duration (in seconds) of the shake effect.
        //   - Parameter 2 (double intensity): Base intensity of the shake effect.
        if (auto shakeParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeStart");
            shakeParams.first > 0.0 && shakeParams.second > 0.0)
        {
            m_shakeEffects.push_back({ static_cast<float>(shakeParams.first), 0.0f,
                                       static_cast<float>(shakeParams.second), false });
            Log::Info("Queued standard shake effect.");
        }

        // Lerp (ramped) shake effect.
        // Message: "CameraShakeLerpStart"
        // Parameters:
        //   - Parameter 1 (double duration): Duration (in seconds) of the shake effect.
        //   - Parameter 2 (double intensity): Base intensity, which will be ramped (lerped) up then down.
        if (auto shakeLerpParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeLerpStart");
            shakeLerpParams.first > 0.0 && shakeLerpParams.second > 0.0)
        {
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
        // Parameters:
        //   - Parameter 1 (double lerp duration): Time for ramping to/from target.
        //   - Parameter 2 (double hold duration): Time to hold at the target zoom.
        //   - Parameter 3 (double targetOrthoWidth): The target orthographic width.
        // The effect lerps from the current camera width to the target and then returns to the base.
        if (auto zoomAutoParams = Application::MessagingSystem::Receive<std::tuple<double, double, double>>("CameraZoomAutoReturnStart");
            std::get<0>(zoomAutoParams) > 0.0)
        {
            float lerpDuration = static_cast<float>(std::get<0>(zoomAutoParams));
            float holdDuration = static_cast<float>(std::get<1>(zoomAutoParams));
            float targetOrthoWidth = static_cast<float>(std::get<2>(zoomAutoParams));
            float currentOrthoWidth = m_zoomBase;
            if (m_mainCameraEntity) 
            {
                if (auto cam = m_mainCameraEntity.GetComponent<Camera>())
                    currentOrthoWidth = cam->GetOrthoWidth();
            }
            m_zoomEffect = ZoomEffect
            {
                0.0f, // 'duration' not used for auto-return zoom
                0.0f,
                targetOrthoWidth,
                currentOrthoWidth, // starting from current camera width
                true,              // enable auto-return effect
                lerpDuration,
                holdDuration,
                2 * lerpDuration + holdDuration // total duration for auto-return (ramp up + hold + ramp down)
            };
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
                // Ramp up then down.
                float factor = (progress < 0.5f) ? (progress * 2.0f) : ((1.0f - progress) * 2.0f);
                intensity *= factor;
            }
            cumulativeShake += GenerateShakeOffset(intensity);
            if (effect.elapsed >= effect.duration) {
                m_shakeEffects.erase(m_shakeEffects.begin() + i);
                Log::Info("Shake effect completed.");
                Application::MessagingSystem::Send("CameraShakeCompleted", 1);
            }
        }

        // ----------------------------------------------------------------
        // Process active zoom effect.
        // ----------------------------------------------------------------
        float delta = 0.0f;
        if (m_zoomEffect.autoReturn) 
        {
            m_zoomEffect.elapsed += deltaTime;
            if (m_zoomEffect.elapsed < m_zoomEffect.lerpDuration) 
            {
                // Ramp up phase: lerp from initial to target.
                float t = m_zoomEffect.elapsed / m_zoomEffect.lerpDuration;
                delta = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * t;
            }
            else if (m_zoomEffect.elapsed < m_zoomEffect.lerpDuration + m_zoomEffect.holdDuration) 
            {
                // Hold phase: fully at target zoom.
                delta = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth);
            }
            else if (m_zoomEffect.elapsed < m_zoomEffect.totalDuration) 
            {
                // Ramp down phase: lerp back to the initial (base) width.
                float t = (m_zoomEffect.elapsed - (m_zoomEffect.lerpDuration + m_zoomEffect.holdDuration)) / m_zoomEffect.lerpDuration;
                delta = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * (1.0f - t);
            }
            else 
            {
                // Auto-return effect completed.
                delta = 0.0f;
                Log::Info("Zoom effect completed.");
                Application::MessagingSystem::Send("CameraZoomCompleted", 1);
            }
        }
        else 
        {
            m_zoomEffect.elapsed += deltaTime;
            float progress = std::min(m_zoomEffect.elapsed / m_zoomEffect.duration, 1.0f);
            delta = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * progress;
            if (m_zoomEffect.elapsed >= m_zoomEffect.duration)
            {
                Log::Info("Zoom effect completed.");
                Application::MessagingSystem::Send("CameraZoomCompleted", 1);
            }
        }

        float effectiveOrthoWidth = std::max(m_zoomBase + delta, m_minOrthoWidth);
        float effectiveOrthoHeight = effectiveOrthoWidth / m_baseAspectRatio;

        // ----------------------------------------------------------------
        // Apply shake and zoom effects to the main camera.
        // ----------------------------------------------------------------
        if (m_mainCameraEntity) 
        {
            if (auto pos = m_mainCameraEntity.GetComponent<Position>())
                pos->position = m_originalCameraPos + cumulativeShake;
            else
                Log::Warning("Main camera Position component missing.");

            if (auto cam = m_mainCameraEntity.GetComponent<Camera>()) 
            {
                cam->SetOrthographic(-effectiveOrthoWidth / 2, effectiveOrthoWidth / 2,
                                     -effectiveOrthoHeight / 2, effectiveOrthoHeight / 2);
                cam->Update();
            }
            else 
            {
                Log::Warning("Main camera Camera component missing.");
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
