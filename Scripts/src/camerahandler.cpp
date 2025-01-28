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
    class CameraHandler : public Script
    {
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
        bool ValidateCamera(FlexECS::EntityID entityID) const;

    public:
        CameraHandler() { ScriptRegistry::RegisterScript(this); }
        std::string GetName() const override { return "CameraHandler"; }

        void Start() override
        {
            //Maybe register all cameras in scene
        }

        void Update() override
        {
            ValidateCamera(1);
            //for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            //{
            //    if (!element.GetComponent<Transform>()->is_active)
            //        continue;

            //    RegisterCamera(element.Get(), *element.GetComponent<Camera>());
            //}
        }

        void Stop() override
        {
            //maybe unregister all cameras in scene
        }
    };

    // Static instance to ensure registration
    static CameraHandler CamHandler;

    void CameraHandler::RegisterCamera(FlexECS::EntityID entityID, const Camera& cam)
    {
        CameraManager::SetCameraRef(entityID, cam);
    }

    void CameraHandler::UnregisterCamera(FlexECS::EntityID entityID)
    {
        CameraManager::RemoveCamera(entityID);
    }

    //In future, this function is meant to update the camera data by running through all 
    // available cameras in core system, then update the data in bank if is_dirty
    bool CameraHandler::ValidateCamera(FlexECS::EntityID entityID) const
    {
        return CameraManager::HasCamera(entityID);
    }
}