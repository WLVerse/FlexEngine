#pragma once

#include "camera.h"
#include "FlexECS/datastructures.h"
#include <unordered_map>
#include <limits>

namespace FlexEngine
{
    constexpr uint64_t INVALID_ENTITY_ID = std::numeric_limits<uint64_t>::max();

    class __FLX_API CameraManager
    {
        // Storage for cameras, using raw pointers to manage lifetime
        static std::unordered_map<FlexECS::EntityID, Camera*> m_cameraEntities;

        // Reserved editor camera ID
        static FlexECS::EntityID m_editorCameraID;

    public:
        // Initializes the CameraManager and sets up the default editor camera
        static void Initialize();

        /*!************************************************************************
         * \brief Adds or updates a camera associated with the given entity ID.
         * \param entityID The ID of the entity to associate with the camera.
         * \param cam The camera to associate with the entity.
         *************************************************************************/
        static void SetCamera(FlexECS::EntityID entityID, Camera* const& cam);

        /*!************************************************************************
         * \brief Removes the camera associated with the given entity ID.
         * \param entityID The ID of the entity to remove.
         *************************************************************************/
        static void RemoveCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Retrieves the camera associated with the given entity ID.
         * \param entityID The ID of the entity to retrieve.
         * \return A pointer to the camera if it exists, nullptr otherwise.
         *************************************************************************/
        static Camera* GetCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Checks if a camera is registered with the given entity ID.
         * \param entityID The ID of the entity to check.
         * \return True if the camera exists, false otherwise.
         *************************************************************************/
        static bool HasCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Clears all registered cameras.
         *************************************************************************/
        static void Clear();

        /*!************************************************************************
         * \brief Gets the default editor camera.
         * \return A pointer to the editor camera.
         *************************************************************************/
        static Camera* GetEditorCamera();
    };
}
