#pragma once

#include "camera.h"
#include "FlexECS/datastructures.h"
#include <unordered_map>
#include <algorithm>
#include <limits>

namespace FlexEngine
{
    constexpr uint64_t INVALID_ENTITY_ID = std::numeric_limits<uint64_t>::max();

    class __FLX_API CameraManager
    {
        // Storage for cameras, using raw pointers to manage lifetime
        static std::unordered_map<FlexECS::EntityID, Camera*> m_cameraEntities;
        static FlexECS::EntityID m_mainGameCameraID;

    public:
        static bool has_main_camera;

        // Tries to find camera if there is none
        static void TryMainCamera();

        /*!************************************************************************
        * \brief Gets the Main Game camera if available.
        * \return A pointer to the Main Game camera.
        *************************************************************************/
        static Camera* GetMainGameCamera();
        static FlexECS::EntityID GetMainGameCameraID();

        static const Vector4& GetMouseToWorld();

        static void RemoveMainCamera();
        
        /*!************************************************************************
        * \brief Sets the main game camera id.
        * \return void.
        *************************************************************************/
        static void SetMainGameCameraID(FlexECS::EntityID id);
    };
}
