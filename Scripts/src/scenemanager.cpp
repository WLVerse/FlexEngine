#include <FlexEngine.h>
using namespace FlexEngine;

//Meant to provide convenient methods for gameplay-related camera tasks, such as cinematic transitions, camera shaking, or following entities.
//For example, through bindings or APIs like SetMainCamera(entityID) or FollowEntity(entityID).
class SceneManagerScript : public Script
{
    //const CameraManager m_camManager;
public:
    SceneManagerScript() { ScriptRegistry::RegisterScript(this); }
    std::string GetName() const override { return "SceneManager"; }

    void Start() override
    {
        Log::Debug("SceneManager: Start");

        //m_camManager
    }

    void Update() override
    {
        //Validate Cameras here
    }

    void Stop() override
    {
        Log::Debug("SceneManager: Stop");
    }
};

// Static instance to ensure registration
static SceneManagerScript SceneManager;
