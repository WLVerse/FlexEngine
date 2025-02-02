#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
using namespace FlexEngine;

namespace FlexEngine
{
    /*!************************************************************************
    * \class CameraHandler
    * \brief This script is intended to handle additional camera functionalities
    * such as camera shake, lerping, and smooth transitions between cameras.
    *
    * \note Camera registration and main camera management have been moved to
    * CameraSystemLayer.
    *
    * \usage Implement this script where camera effects like shake or transitions
    * are needed. This could be used in conjunction with CameraManager to modify
    * camera properties dynamically.
    *************************************************************************/
    class CameraHandler : public IScript
    {
    public:
        CameraHandler()
        {
            ScriptRegistry::RegisterScript(this);
        }

        std::string GetName() const override { return "CameraHandler"; }

        void Awake() override {}

        void Start() override
        {
            // Pseudo-code:
            // - Initialize camera effect parameters (shake intensity, lerp speed, etc.)
            // - Ensure dependencies like CameraManager are available
        }

        void Update() override
        {
            // Pseudo-code:
            // - If camera shake is active:
            //     - Apply random offset to camera position within intensity range
            // - If transitioning between cameras:
            //     - Interpolate position and rotation from current to target camera
            // - Apply any other effects such as zoom or dynamic FOV changes
        }

        void Stop() override
        {
            // Pseudo-code:
            // - Clean up camera effects
            // - Reset any modified camera properties to their default states
        }
    };

    // Static instance to ensure registration
    static CameraHandler CamHandler;
}
