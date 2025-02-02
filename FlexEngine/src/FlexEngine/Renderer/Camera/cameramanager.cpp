#include "cameramanager.h"
#include <stdexcept>


namespace FlexEngine
{
    #pragma region Static Member Initialization

    std::unordered_map<FlexECS::EntityID, Camera*> CameraManager::m_cameraEntities;
    FlexECS::EntityID CameraManager::m_editorCameraID = INVALID_ENTITY_ID;
    FlexECS::EntityID CameraManager::m_mainGameCameraID = INVALID_ENTITY_ID;
    #pragma endregion

    #pragma region Camera Lifetime Management

    void CameraManager::Initialize()
    {
        try
        {
            Log::Debug("Initializing CameraManager...");
            // Create the default editor camera manually
            Camera* editorCamera = new Camera({ 800.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);

            m_editorCameraID = 0; // Assign a unique ID for the editor camera
            Log::Debug("Editor camera created with ID: " + std::to_string(m_editorCameraID));

            m_cameraEntities[m_editorCameraID] = editorCamera;
            Log::Debug("Editor camera registered in CameraManager.");

        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::Initialize: " + std::string(e.what()));
        }
    }

    void CameraManager::SetCamera(FlexECS::EntityID entityID, Camera* const& cam)
    {
        try
        {
            Log::Debug("Setting camera with Entity ID: " + std::to_string(entityID));
            if (!cam)
            {
                throw std::invalid_argument("Camera pointer is null.");
            }
            m_cameraEntities[entityID] = cam;
            Log::Debug("Camera set for Entity ID: " + std::to_string(entityID));

        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::SetCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
        }
    }

    void CameraManager::RemoveCamera(FlexECS::EntityID entityID)
    {
        try
        {
            Log::Debug("Removing camera with Entity ID: " + std::to_string(entityID));
            auto it = m_cameraEntities.find(entityID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Camera with EntityID " + std::to_string(entityID) + " not found.");
            }

            // Manually delete the camera to avoid memory leaks
            delete it->second;
            m_cameraEntities.erase(entityID);
            Log::Debug("Camera removed for Entity ID: " + std::to_string(entityID));
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::RemoveCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
        }
    }

    Camera* CameraManager::GetCamera(FlexECS::EntityID entityID)
    {
        try
        {
            //Log::Debug("Getting camera with Entity ID: " + std::to_string(entityID));
            auto it = m_cameraEntities.find(entityID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Camera with EntityID " + std::to_string(entityID) + " not found.");
            }

            //Log::Debug("Camera found for Entity ID: " + std::to_string(entityID));
            return it->second;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::GetCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
            return nullptr;  // Return null if the camera was not found
        }
    }

    bool CameraManager::HasCamera(FlexECS::EntityID entityID)
    {
        try
        {
            bool exists = m_cameraEntities.find(entityID) != m_cameraEntities.end();
            return exists;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::HasCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
            return false;
        }
    }

    void CameraManager::Clear()
    {
        try
        {
            Log::Debug("Clearing all cameras from CameraManager...");
            // Manager now only handles clearing out references to these cameras
            //// Manually delete all camera objects
            //for (auto& pair : m_cameraEntities)
            //{
            //    delete pair.second;
            //}
            delete m_cameraEntities.find(0)->second;
            m_cameraEntities.clear();
            m_editorCameraID = INVALID_ENTITY_ID;
            Log::Debug("All cameras cleared.");
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::Clear: " + std::string(e.what()));
        }
    }

    void CameraManager::DeregisterECSCams()
    {
        for (auto it = m_cameraEntities.begin(); it != m_cameraEntities.end();)
        {
            if (it->first != 0) // Remove cameras that are NOT entityID = 0
                it = m_cameraEntities.erase(it); // Erase and update iterator
            else
                ++it;
        }
    }

    Camera* CameraManager::GetEditorCamera()
    {
        try
        {
            auto it = m_cameraEntities.find(m_editorCameraID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Editor camera not found.");
            }
            return it->second;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::GetEditorCamera: " + std::string(e.what()));
            return nullptr;  // Return null if the editor camera is not found
        }
    }

    Camera* CameraManager::GetMainGameCamera()
    {
        try
        {
            Log::Debug("Getting main game camera...");
            auto it = m_cameraEntities.find(m_mainGameCameraID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Main Game camera not found.");
            }

            Log::Debug("Main Game camera found.");
            return it->second;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::GetMainGameCamera: " + std::string(e.what()));
            return nullptr;  // Return null if the main game camera is not found
        }
    }

    FlexECS::EntityID CameraManager::GetMainGameCameraID() { return m_mainGameCameraID; }

    void CameraManager::SetMainGameCameraID(FlexECS::EntityID id)
    {
        Log::Debug("Setting main game camera with Entity ID: " + std::to_string(id));
        m_mainGameCameraID = id;
        Log::Debug("Main Game camera set for Entity ID: " + std::to_string(id));
    }
    #pragma endregion
}
