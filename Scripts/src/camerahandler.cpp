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
        FlexECS::EntityID m_mainGameCameraID = INVALID_ENTITY_ID;

        /*!************************************************************************
        * \brief Registers a camera entity with the CameraManager.
        * \param entityID The entity ID of the camera component.
        * \param cameraData Initial camera data.
        *************************************************************************/
        void RegisterCamera(FlexECS::EntityID entityID, const Camera& cameraData) {
            if (!ValidateCamera(entityID))
                CameraManager::SetCameraRef(entityID, cameraData);
        }

        /*!************************************************************************
         * \brief Unregisters a camera entity from the CameraManager.
         * \param entityID The entity ID of the camera component.
         *************************************************************************/
        void UnregisterCamera(FlexECS::EntityID entityID) {
            if (ValidateCamera(entityID))
                CameraManager::RemoveCamera(entityID);
        }

        /*!************************************************************************
         * \brief Validates the presence of a camera in the CameraManager.
         * \param entityID The entity ID to validate.
         * \return True if the camera is valid, false otherwise.
         *************************************************************************/
        bool ValidateCamera(FlexECS::EntityID entityID) const {
            return CameraManager::HasCamera(entityID);
        }

    public:
        CameraHandler() { ScriptRegistry::RegisterScript(this); }
        std::string GetName() const override { return "CameraHandler"; }

        void Awake() override {}

        void Start() override
        {
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
                if (elem.GetComponent<Transform>()->is_active && !ValidateCamera(elem.Get()))
                    RegisterCamera(elem.Get(), *elem.GetComponent<Camera>());
        }

        void Update() override 
        { 
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                if (elem.GetComponent<Transform>()->is_active)
                {
                    if (!ValidateCamera(elem.Get()))
                        RegisterCamera(elem.Get(), *elem.GetComponent<Camera>());
                    if (m_mainGameCameraID == INVALID_ENTITY_ID)
                        m_mainGameCameraID = elem.Get();
                }
            }

        }

        void Stop() override
        {
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
                UnregisterCamera(elem.Get());
        }

        //Will send editor cam id if camID is invalid
        FlexECS::EntityID GetMainGameCameraID() const { return m_mainGameCameraID != INVALID_ENTITY_ID ? m_mainGameCameraID : 0; }
        void SetMainGameCameraID(FlexECS::EntityID id) { if (ValidateCamera(id)) m_mainGameCameraID = id; }
    };

    // Static instance to ensure registration
    static CameraHandler CamHandler;
}