#include <FlexEngine.h>
using namespace FlexEngine;

//Meant to provide convenient methods for gameplay-related camera tasks, such as cinematic transitions, camera shaking, or following entities.
//For example, through bindings or APIs like SetMainCamera(entityID) or FollowEntity(entityID).

namespace FlexEngine
{
    /*!************************************************************************
    * \class CameraHandler
    * \brief Monitors entities with camera components and communicates with
    * the CameraManager to manage their lifetimes.
    *************************************************************************/
    class CameraHandler
    {
    public:
        /*!************************************************************************
         * \brief Registers a camera entity with the CameraManager.
         * \param entityID The entity ID of the camera component.
         * \param cameraData Initial camera data.
         *************************************************************************/
        void RegisterCamera(FlexECS::EntityID entityID, const Camera& cameraData);

        /*!************************************************************************
         * \brief Unregisters a camera entity from the CameraManager.
         * \param entityID The entity ID of the camera component.
         *************************************************************************/
        void UnregisterCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Validates the presence of a camera in the CameraManager.
         * \param entityID The entity ID to validate.
         * \return True if the camera is valid, false otherwise.
         *************************************************************************/
        bool ValidateCameras(FlexECS::EntityID entityID) const;
    };


    void CameraHandler::RegisterCamera(FlexECS::EntityID entityID, const Camera& cameraData)
    {
        CameraManager::Instance().SetCameraData(entityID, cameraData);
    }

    void CameraHandler::UnregisterCamera(FlexECS::EntityID entityID)
    {
        CameraManager::Instance().RemoveCamera(entityID);
    }

    bool CameraHandler::ValidateCameras(FlexECS::EntityID entityID) const
    {
        return CameraManager::Instance().GetCameraData(entityID) != nullptr;
    }
}