#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
using namespace FlexEngine;

namespace FlexEngine
{
    /*!************************************************************************
    * \class CameraHandler
    * \brief Monitors entities with camera components and communicates with
    * the CameraManager to manage their lifetimes.
    *************************************************************************/
    class CameraHandler : public IScript
    {
        /*!************************************************************************
        * \brief Registers a camera entity with the CameraManager.
        * \param entityID The entity ID of the camera component.
        * \param cameraData Initial camera data.
        *************************************************************************/
        void RegisterCamera(FlexECS::EntityID entityID, Camera* cam)
        {
            try {
                if (!ValidateCamera(entityID))
                {
                    CameraManager::SetCamera(entityID, cam);
                    std::stringstream text;
                    text << "Camera with entityID {" << entityID << "} successfully registered.";
                    Log::Info(text.str());
                }
                else
                {
                    std::stringstream text;
                    text << "Camera with entityID {" << entityID << "} is already registered.";
                    Log::Debug(text.str());
                }
            }
            catch (const std::exception& e)
            {
                std::stringstream text;
                text << "Failed to register camera with entityID {" << entityID << "} : " << e.what();
                Log::Error(text.str());
            }
        }

        /*!************************************************************************
         * \brief Unregisters a camera entity from the CameraManager.
         * \param entityID The entity ID of the camera component.
         *************************************************************************/
        void UnregisterCamera(FlexECS::EntityID entityID)
        {
            try
            {
                if (ValidateCamera(entityID))
                {
                    CameraManager::RemoveCamera(entityID);
                    std::stringstream text;
                    text << "Camera with entityID {" << entityID << "} successfully unregistered.";
                    Log::Info(text.str());
                }
                else
                {
                    std::stringstream text;
                    text << "Camera with entityID {" << entityID << "} is not registered.";
                    Log::Debug(text.str());
                }
            }
            catch (const std::exception& e)
            {
                std::stringstream text;
                text << "Failed to unregister camera with entityID {" << entityID << "} : " << e.what();
                Log::Error(text.str());
            }
        }

        /*!************************************************************************
         * \brief Validates the presence of a camera in the CameraManager.
         * \param entityID The entity ID to validate.
         * \return True if the camera is valid, false otherwise.
         *************************************************************************/
        bool ValidateCamera(FlexECS::EntityID entityID) const
        {
            return CameraManager::HasCamera(entityID);
        }

    public:
        CameraHandler()
        {
            ScriptRegistry::RegisterScript(this);
        }

        std::string GetName() const override { return "CameraHandler"; }

        void Awake() override {}

        void Start() override
        {
            try
            {
                for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
                {
                    if (elem.GetComponent<Transform>()->is_active && !ValidateCamera(elem.Get()))
                    {
                        RegisterCamera(elem.Get(), elem.GetComponent<Camera>());
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::stringstream text;
                text << "Error during CameraHandler::Start: {" << e.what() << "}";
                Log::Error(text.str());
            }
        }

        void Update() override
        {
            try
            {
                bool foundActiveCamera = false;

                // Iterate through cameras in the active scene
                for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
                {
                    // Check if the camera is active
                    if (elem.GetComponent<Transform>()->is_active)
                    {
                        if (!ValidateCamera(elem.Get()))
                            RegisterCamera(elem.Get(), elem.GetComponent<Camera>());

                        // Set the first valid camera as the main game camera if not already set
                        if (CameraManager::GetMainGameCameraID() == INVALID_ENTITY_ID)
                            CameraManager::SetMainGameCameraID(elem.Get());

                        foundActiveCamera = true;
                    }
                    else
                    {
                        // If camera is inactive, mark as invalid
                        if (CameraManager::GetMainGameCameraID() == elem.Get())
                        {
                            CameraManager::SetMainGameCameraID(INVALID_ENTITY_ID);
                            Log::Info("Main game camera is now invalid due to inactivity.");
                        }
                    }
                }

                // If no active camera found, reset main camera ID to INVALID_ENTITY_ID
                if (!foundActiveCamera)
                {
                    CameraManager::SetMainGameCameraID(INVALID_ENTITY_ID);
                    Log::Info("No active camera found, main game camera set to INVALID.");
                }
            }
            catch (const std::exception& e)
            {
                std::stringstream text;
                text << "Error during CameraHandler::Update: {" << e.what() << "}";
                Log::Error(text.str());
            }
        }

        void Stop() override
        {
            try
            {
                Log::Info("Stopping and unregistering cameras...");
                for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
                    UnregisterCamera(elem.Get());
            }
            catch (const std::exception& e)
            {
                std::stringstream text;
                text << "Error during CameraHandler::Stop: {" << e.what() << "}";
                Log::Error(text.str());
            }
        }
    };

    // Static instance to ensure registration
    static CameraHandler CamHandler;
}
