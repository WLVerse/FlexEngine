#include "CameraSystemLayer.h"
#include <cstdlib>    // For std::rand()
#include <ctime>      // For std::time()
#include <algorithm>  // For std::min and std::max

namespace Game 
{

    CameraSystemLayer::CameraSystemLayer() : Layer("Camera System Layer") 
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

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

        // ----------------------------------------------------------------
        // Enqueue new camera effects from messages.
        // ----------------------------------------------------------------

        // --- SHAKE EFFECTS ---
        // Standard shake effect.
        // Message: "CameraShakeStart"
        // Parameters: (duration, intensity)
        auto shakeParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeStart");
        if (shakeParams.first > 0.0 && shakeParams.second > 0.0)
        {
           // Enqueue standard shake effect.
           CameraManager::GetMainGameCamera()->EnqueueShakeEffect(static_cast<float>(shakeParams.first),
                                        static_cast<float>(shakeParams.second),
                                        false);
           Log::Info("Queued standard shake effect.");
        }

        // Lerp (ramped) shake effect.
        // Message: "CameraShakeLerpStart"
        // Parameters: (duration, intensity)
        auto shakeLerpParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraShakeLerpStart");
        if (shakeLerpParams.first > 0.0 && shakeLerpParams.second > 0.0)
        {
            CameraManager::GetMainGameCamera()->EnqueueShakeEffect(static_cast<float>(shakeLerpParams.first),
                                        static_cast<float>(shakeLerpParams.second),
                                        true);
                Log::Info("Queued lerp shake effect.");
        }

        // Persistent zoom effect.
        // Message: "CameraZoomStart"
        // Parameters:
        //   - Parameter 1 (double duration): Duration (in seconds) over which the zoom occurs.
        //   - Parameter 2 (double targetOrthoWidth): The target orthographic width to achieve.
        auto zoomParams = Application::MessagingSystem::Receive<std::pair<double, double>>("CameraZoomStart");
        if (zoomParams.first > 0.0)
        {
            CameraManager::GetMainGameCamera()->EnqueueZoomEffect(static_cast<float>(zoomParams.first),
                                   static_cast<float>(zoomParams.second),
                                   false);
            Log::Info("Queued persistent zoom effect.");
        }

        // Auto-return zoom effect.
        // Message: "CameraZoomAutoReturnStart"
        // Parameters: (lerpDuration, holdDuration, targetOrthoWidth)
        auto zoomAutoParams = Application::MessagingSystem::Receive<std::tuple<double, double, double>>("CameraZoomAutoReturnStart");
        if (std::get<0>(zoomAutoParams) > 0.0)
        {
            float lerpDuration = static_cast<float>(std::get<0>(zoomAutoParams));
            float holdDuration = static_cast<float>(std::get<1>(zoomAutoParams));
            float targetOrthoWidth = static_cast<float>(std::get<2>(zoomAutoParams));
            CameraManager::GetMainGameCamera()->EnqueueZoomEffect(0.0f, // duration not used for auto-return
                                   targetOrthoWidth,
                                   true,
                                   lerpDuration,
                                   holdDuration);
            Log::Info("Queued auto-return zoom effect.");
        }

        // ----------------------------------------------------------------
        // Update all cameras in the scene.
        // ----------------------------------------------------------------
        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Camera>())
        {
            elem.GetComponent<Camera>()->SetViewMatrix(elem.GetComponent<Position>()->position);
            elem.GetComponent<Camera>()->Update(Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime());
        }
    }

} // namespace Game
