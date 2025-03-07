#include "CameraSystemLayer.h"
#include <cstdlib>    // For std::rand()
#include <ctime>      // For std::time()
#include <algorithm>  // For std::min and std::max

namespace Game {

    CameraSystemLayer::CameraSystemLayer() : Layer("Camera System Layer") {
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

        // Persistent zoom effect.
        // Message: "CameraZoomStart"
        // Parameters:
        //   - Parameter 1 (double duration): Duration (in seconds) over which the zoom occurs.
        //   - Parameter 2 (double targetOrthoWidth): The target orthographic width to achieve by the end of the effect.
        // A new zoom message replaces any existing zoom effect.
        if (auto zoomParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraZoomStart");
            zoomParams.first > 0.0)
        {
            m_zoomEffect = ZoomEffect({ static_cast<float>(zoomParams.first), 0.0f,
                                      static_cast<float>(zoomParams.second), m_zoomBase, false });
            Log::Info("Queued persistent zoom effect.");
        }

        // Auto-return zoom effect.
        // Message: "CameraZoomAutoReturnStart"
        // Parameters:
        //   - Parameter 1 (double duration): Duration (in seconds) for the auto-return zoom effect.
        //   - Parameter 2 (double targetOrthoWidth): The target orthographic width to reach before returning to the base width.
        // A new zoom message replaces any existing zoom effect.
        if (auto zoomAutoParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraZoomAutoReturnStart");
            zoomAutoParams.first > 0.0)
        {
            m_zoomEffect = ZoomEffect({ static_cast<float>(zoomAutoParams.first), 0.0f,
                                      static_cast<float>(zoomAutoParams.second), m_zoomBase, true });
            Log::Info("Queued auto-return zoom effect.");
        }

        // ----------------------------------------------------------------
        // Process active shake effects.
        // ----------------------------------------------------------------
        Vector3 cumulativeShake(0.0f);
        for (int i = static_cast<int>(m_shakeEffects.size()) - 1; i >= 0; --i) {
            auto& effect = m_shakeEffects[i];
            effect.elapsed += deltaTime;
            float intensity = effect.intensity;
            if (effect.lerp) {
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
         m_zoomEffect.elapsed += deltaTime;
         float progress = std::min(m_zoomEffect.elapsed / m_zoomEffect.duration, 1.0f);
         float delta = 0.0f;
         if (m_zoomEffect.autoReturn) 
         {
             float factor = (progress < 0.5f) ? (progress * 2.0f) : ((1.0f - progress) * 2.0f);
             delta = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * factor;
         }
         else 
         {
             delta = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * progress;
         }
         if (m_zoomEffect.elapsed >= m_zoomEffect.duration)
         {
             Log::Info("Zoom effect completed.");
             Application::MessagingSystem::Send("CameraZoomCompleted", 1);
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
        if (auto scene = FlexECS::Scene::GetActiveScene()) 
        {
            for (auto& elem : scene->CachedQuery<Position, Camera>())
            {
                if (auto pos = elem.GetComponent<Position>(); pos) 
                {
                    if (auto cam = elem.GetComponent<Camera>(); cam) 
                    {
                        cam->SetViewMatrix(pos->position);
                        cam->Update();
                    }
                }
            }
        }
    }

} // namespace Game
