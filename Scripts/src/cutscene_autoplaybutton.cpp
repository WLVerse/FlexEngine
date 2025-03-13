/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// cutscene_autoplaybutton.cpp
//
// This script handles the interactions for the cutscene autoplay button.
// It listens for mouse events and dispatches messaging events accordingly,
// such as hovering and clicking actions during a cutscene.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
using namespace FlexEngine;

class Cutscene_AutoplayBtnScript : public IScript
{
public:
    // Constructor: Registers this script with the ScriptRegistry.
    Cutscene_AutoplayBtnScript()
    {
        ScriptRegistry::RegisterScript(this);
    }

    // Returns the unique name of this script.
    std::string GetName() const override
    {
        return "Cutscene_AutoplayBtn";
    }

    // Called every frame to update the script.
    // Currently, no per-frame update is needed.
    void Update() override
    {
    }

    // Called when the mouse enters the area of the autoplay button.
    // Sends a message to indicate that the button is being hovered.
    void OnMouseEnter() override
    {
        Application::MessagingSystem::Send("Cutscene_AutoplayBtn hovered", true);
    }

    // Called while the mouse remains over the autoplay button.
    // Checks for a left mouse button click to trigger a click event.
    void OnMouseStay() override
    {
        if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
        {
            Application::MessagingSystem::Send("Cutscene_AutoplayBtn clicked", true);
        }
    }

    // Called when the mouse exits the autoplay button area.
    // Currently, no action is taken on mouse exit.
    void OnMouseExit() override
    {
    }
};

// Static instance to ensure registration of the Cutscene_AutoplayBtnScript.
static Cutscene_AutoplayBtnScript Cutscene_AutoplayBtn;
