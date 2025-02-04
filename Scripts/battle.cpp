#include <FlexEngine.h>
using namespace FlexEngine;

class BattleScript : public IScript
{
public:
    BattleScript() { ScriptRegistry::RegisterScript(this); }
    std::string GetName() const override { return "BattleTest"; }
    std::vector<void (*)()> effects;
    void Awake() override
    {

    }

    void Start() override
    {
        Log::Info("PlayAnimationScript attached to entity: " + FLX_STRING_GET(*self.GetComponent<EntityName>()) + ". This script will animate sprites.");

        self.GetComponent<Animator>()->should_play = false;
    }

    void Update() override
    {
        if (Input::AnyKey())
        {
            self.GetComponent<Animator>()->should_play = true;
        }
        else
        {
            self.GetComponent<Animator>()->should_play = false;
        }
    }

    /*void CreateIcon(void (*function)())
    {
        effects.push_back(function);
    }*/

    void Stop() override
    {
        self.GetComponent<Animator>()->should_play = true;
    }
};

// Static instance to ensure registration
static BattleScript BattleTest;
