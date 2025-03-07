// WLVERSE [https://wlverse.web.app]
// baselayer.cpp
// 
// Base layer that runs before everything.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Game
{

    void CutsceneLayer::OnAttach()
    {
        File& file = File::Open(Path::current("assets/saves/cutscene.flxscene"));
        FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

        loadCutscene(FLX_STRING_NEW(R"(/cutscenes/OpeningCutscene.flxdialogue)"),
                     FLX_STRING_NEW(R"(/cutscenes/OpeningCutscene.flxcutscene)"));

        m_currFrameIndex = 0;
        m_currSectionIndex = 0;

        // Create the current shot entity.
       // Intentionally setting sprite handle to 0.
        m_currShot = FlexECS::Scene::GetActiveScene()->GetEntityByName("Current Shot");
        m_currShot.GetComponent<Sprite>()->sprite_handle = 0;

        // Create the next shot entity and assign the image if available.
        m_nextShot = FlexECS::Scene::GetActiveScene()->GetEntityByName("Next Shot");
        if (m_currFrameIndex + 1 < m_CutsceneImages.size())
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currFrameIndex];

        m_dialoguebox = FlexECS::Scene::GetActiveScene()->GetEntityByName("Dialogue Box");
        m_dialoguebox.GetComponent<Text>()->textboxDimensions = Vector2(Application::GetCurrentWindow()->GetWidth() * 0.8f, 70.0f);
        m_shadowdialoguebox = FlexECS::Scene::GetActiveScene()->GetEntityByName("Shadow Dialogue Box");
        m_shadowdialoguebox.GetComponent<Text>()->textboxDimensions = Vector2(Application::GetCurrentWindow()->GetWidth() * 0.8f, 70.0f);
        m_dialoguearrow = FlexECS::Scene::GetActiveScene()->GetEntityByName("Dialogue Arrow");

        m_autoplayText = FlexECS::Scene::GetActiveScene()->GetEntityByName("Autoplay Text");
        m_autoplayBtn = FlexECS::Scene::GetActiveScene()->GetEntityByName("Autoplay");
        m_autoplaySymbolAuto = FlexECS::Scene::GetActiveScene()->GetEntityByName("Autoplay Symbol Auto");
        m_autoplaySymbolPlaying = FlexECS::Scene::GetActiveScene()->GetEntityByName("Autoplay Symbol Playing");

        m_skiptext = FlexECS::Scene::GetActiveScene()->GetEntityByName("Skip Text");
        m_instructiontxt = FlexECS::Scene::GetActiveScene()->GetEntityByName("Instruction Text");
        m_instructiontxtopacityblk = FlexECS::Scene::GetActiveScene()->GetEntityByName("Instruction Text Opacity Block");
        m_skipwheel = FlexECS::Scene::GetActiveScene()->GetEntityByName("Skip Wheel");
        
        auto& font = FLX_ASSET_GET(Asset::Font, R"(/fonts/Electrolize/Electrolize-Regular.ttf)");
        font.SetFontSize(30);

        Application::MessagingSystem::Send("TransitionStart", std::pair<int,double>{ 1,1.0 });
    }

    void CutsceneLayer::OnDetach()
    {
        // Cleanup code, if needed.
        auto& font = FLX_ASSET_GET(Asset::Font, R"(/fonts/Electrolize/Electrolize-Regular.ttf)");
        font.SetFontSize(50);
    }

    void CutsceneLayer::StartCutscene()
    {
        m_CutsceneActive = true;

        // Reset timers and transition phase.
        UpdateTimings(false);
    }

    void CutsceneLayer::StopCutscene()
    {
        m_CutsceneActive = false;

        m_currShot.GetComponent<Transform>()->is_active = false;
        m_nextShot.GetComponent<Transform>()->is_active = false;
        //Additional logic
        Application::MessagingSystem::Send("Start Game", true);
    }

    void CutsceneLayer::RestartCutscene()
    {

        // Reload the cutscene assets.
        loadCutscene(FLX_STRING_NEW(R"(/cutscenes/OpeningCutscene.flxdialogue)"),
                     FLX_STRING_NEW(R"(/cutscenes/OpeningCutscene.flxcutscene)"));

        // Reset all indices and timers.
        m_currSectionIndex = 0;
        m_currFrameIndex = 0;
        m_currDialogueIndex = 0;
        m_ElapsedTime = 0.0f;
        m_TransitionElapsedTime = 0.0f;
        m_dialogueTimer = 0.0f;
        m_dialogueHoldTimer = 0.0f;
        m_TransitionPhase = TransitionPhase::None;

        // Update the sprite components for the current and next shots.
        auto* currSprite = m_currShot.GetComponent<Sprite>();
        auto* nextSprite = m_nextShot.GetComponent<Sprite>();

        // If there are cutscene images available, assign the first image to the current shot
        // and the second image (if available) to the next shot.
        if (!m_CutsceneImages.empty())
        {
            currSprite->sprite_handle = m_CutsceneImages[0];
            nextSprite->sprite_handle = (m_CutsceneImages.size() > 1) ? m_CutsceneImages[1] : 0;
        }
        else
        {
            currSprite->sprite_handle = 0;
            nextSprite->sprite_handle = 0;
        }

        // Reset opacities to ensure full visibility.
        currSprite->opacity = 1.0f;
        nextSprite->opacity = 1.0f;

        // Mark the cutscene as active and restart it.
        m_CutsceneActive = true;
        StartCutscene();
    }

    void CutsceneLayer::loadCutscene(FlexECS::Scene::StringIndex dialogue_file, FlexECS::Scene::StringIndex cutscene_file)
    {
        // Retrieve the dialogue and cutscene assets.
        auto& asset_dialogue = FLX_ASSET_GET(Asset::Dialogue, FLX_STRING_GET(dialogue_file));
        auto& asset_cutscene = FLX_ASSET_GET(Asset::Cutscene, FLX_STRING_GET(cutscene_file));
        m_currDialogueFile = dialogue_file;
        m_currCutsceneFile = cutscene_file;

        // Clear the current cutscene images vector and dialogue vector.
        m_CutsceneImages.clear();
        m_CutsceneDialogue.clear();

        // Iterate through the dialogue asset's ordered vector.
        for (const auto& entry : asset_dialogue.dialogues)
        {
            const std::string& cutsceneName = entry.cutsceneName;

            // Look up the corresponding cutscene info using the cutscene name.
            auto it = asset_cutscene.cutscenes.find(cutsceneName);
            if (it == asset_cutscene.cutscenes.end())
            {
                Log::Error("CutsceneLayer::loadCutscene: Cutscene \"" + cutsceneName + "\" not found.");
                continue;
            }

            const auto& info = it->second;

            // For a single frame cutscene, simply load the image without a frame number.
            if (info.frameStart == info.frameEnd)
            {
                std::string path = "/cutscenes/" + cutsceneName + ".png";
                m_CutsceneImages.push_back(FLX_STRING_NEW(path.c_str()));
            }
            else
            {
                // For multiple frames, generate and add each frame's image path in order.
                for (int frame = info.frameStart; frame <= info.frameEnd; ++frame)
                {
                    char buffer[256];
                    // Use 5-digit zero padding for frame numbers.
                    snprintf(buffer, sizeof(buffer), "/cutscenes/%s/%s_%05d.png", cutsceneName.c_str(), cutsceneName.c_str(), frame);
                    m_CutsceneImages.push_back(FLX_STRING_NEW(buffer));
                }
            }

            // For conversion into Flx_String for easy retrival
            for (const auto& block : entry.blocks)
            {
                std::vector<FlexECS::Scene::StringIndex> dialogueBlock;
                for (const auto& line : block)
                {
                    dialogueBlock.push_back(FLX_STRING_NEW(line.c_str()));
                }
                m_CutsceneDialogue.push_back(dialogueBlock);
            }
        }

        // Update the timings of cutscene
        UpdateTimings(false);
    }

    // Main update function now becomes a high-level coordinator.
    void CutsceneLayer::Update()
    {
        if (m_currShot == FlexECS::Entity::Null || m_nextShot == FlexECS::Entity::Null)
        {
            Log::Debug("Cutscene Shots have been deleted. Please do not delete them.");
            return;
        }
        
        // Handles Transition Messages
        int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
        if (transitionMSG == 1)
            StartCutscene();
        else if (transitionMSG == 2)
            StopCutscene();

        if (!m_CutsceneActive)
            return;

        float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // Update UI animations:
        updateInstructionAnimation(dt);  // for pre-transition instruction text
        updateDialogueArrow(dt);         // for dialogue arrow in manual mode
        //updateSkipUI(dt);                // for skip text and wheel

        // Process global input (restart, escape).
        processGlobalInput();


        // Update dialogue based on the current mode.
        if (is_autoplay)
            updateDialogueAuto(dt);
        else
            updateDialogueManual(dt);

        // Update image frames.
        updateImageFrames(dt);

        // Update transition effects.
        updateTransitionPhase(dt);
    }

    #pragma region Helper Func
    // Process input that applies globally across modes.
    void CutsceneLayer::processGlobalInput()
    {
        if (Input::GetKeyDown(GLFW_KEY_R))
            RestartCutscene();
        if (Input::GetKey(GLFW_KEY_ESCAPE))
            Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 0.5 });

        bool autoplaybtn_click = Application::MessagingSystem::Receive<bool>("Cutscene_AutoplayBtn clicked");
        bool autoplaybtn_hover = Application::MessagingSystem::Receive<bool>("Cutscene_AutoplayBtn hovered");

        if (autoplaybtn_hover || autoplaybtn_click)
            enable_clickingreaction = false;
        else
            enable_clickingreaction = true;

        if (autoplaybtn_click)
        {
            // Swap the polarity of is_autoplay
            is_autoplay = !is_autoplay;

            // Change the symbols accordingly
            m_autoplayText.GetComponent<Text>()->text = is_autoplay ? FLX_STRING_NEW("Playing") : FLX_STRING_NEW("Auto");
            m_autoplaySymbolAuto.GetComponent<Transform>()->is_active = !is_autoplay;
            m_autoplaySymbolPlaying.GetComponent<Transform>()->is_active = is_autoplay;
        }
    }

    void CutsceneLayer::UpdateTimings(bool toNextSection)
    {
        // Retrieve the dialogue and cutscene assets.
        auto& dialogueAsset = FLX_ASSET_GET(Asset::Dialogue, FLX_STRING_GET(m_currDialogueFile));
        auto& cutsceneAsset = FLX_ASSET_GET(Asset::Cutscene, FLX_STRING_GET(m_currCutsceneFile));

        // If moving to the next section, update section and reset the current frame index.
        if (toNextSection)
        {
            // Advance to the next section and reset the frame index.
            m_currSectionIndex++;
            m_currFrameIndex = 0;
        }

        // Ensure we haven't run past the dialogue entries.
        if (m_currSectionIndex >= dialogueAsset.dialogues.size())
        {
            Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 0.5 });
            return;
        }

        // Get the current dialogue entry.
        // Assumes each DialogueEntry has a member "cutsceneName" matching the key in cutsceneAsset.cutscenes.
        const auto& dialogueEntry = dialogueAsset.dialogues[m_currSectionIndex];
        const std::string& cutsceneName = dialogueEntry.cutsceneName;

        // Look up the cutscene info based on the cutscene name.
        auto it = cutsceneAsset.cutscenes.find(cutsceneName);
        if (it != cutsceneAsset.cutscenes.end())
        {
            const auto& info = it->second;
            // Total section duration.
            m_ImageDuration = info.duration;
            m_PreTransitionDuration = info.preTime;
            m_PostTransitionDuration = info.postTime;

            // Compute the number of frames in this section.
            m_frameCount = (info.frameEnd - info.frameStart) + 1;
            m_PerFrameDuration = (m_frameCount > 0) ? (m_ImageDuration / static_cast<float>(m_frameCount)) : m_ImageDuration;
        }
        else
        {
            // Use default timings if no matching cutscene info is found.
            m_ImageDuration = 0.6f;
            m_PreTransitionDuration = 1.0f;
            m_PostTransitionDuration = 1.0f;
            m_PerFrameDuration = m_ImageDuration;
            m_frameCount = 1;
        }

        // Reset timers and transition phase.
        m_ElapsedTime = 0.0f;
        m_TransitionElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;
    }

    // Update the dialogue text animation (common to both auto and manual modes).
    void CutsceneLayer::updateDialogueText(float dt)
    {
        if (m_currSectionIndex < m_CutsceneDialogue.size())
        {
            const auto& dialogueBlock = m_CutsceneDialogue[m_currSectionIndex];
            if (m_currDialogueIndex < dialogueBlock.size())
            {
                std::string fullText = FLX_STRING_GET(dialogueBlock[m_currDialogueIndex]);
                size_t totalChars = fullText.size();
                m_dialogueTimer += dt;
                size_t charsToShow = static_cast<size_t>(m_dialogueTimer * m_dialogueTextRate);
                if (charsToShow > totalChars)
                {
                    charsToShow = totalChars;
                    m_dialogueIsWaitingForInput = true;
                }
                std::string displayedText = fullText.substr(0, charsToShow);
                FlexECS::Scene::StringIndex txt = FLX_STRING_NEW(displayedText);
                m_dialoguebox.GetComponent<Text>()->text = txt;
                m_shadowdialoguebox.GetComponent<Text>()->text = txt;
            }
        }
    }

    // Advance the dialogue line and trigger a transition if the dialogue block is done.
    void CutsceneLayer::advanceDialogue()
    {
        m_currDialogueIndex++;
        m_dialogueTimer = 0.0f;
        m_dialogueHoldTimer = 0.0f;
        if (m_currSectionIndex < m_CutsceneDialogue.size())
        {
            const auto& dialogueBlock = m_CutsceneDialogue[m_currSectionIndex];
            if (m_currDialogueIndex >= dialogueBlock.size())
            {
                // Dialogue block complete – skip any remaining frames and trigger transition.
                skipRemainingFrames();
                m_TransitionPhase = TransitionPhase::PreTransition;
                m_TransitionElapsedTime = 0.0f;
            }
        }
    }

    // Remove any unplayed frames and update the current and next shots.
    void CutsceneLayer::skipRemainingFrames()
    {
        size_t remainingFrames = (m_frameCount > m_currFrameIndex) ? (m_frameCount - m_currFrameIndex) - 1 : 0;
        if (remainingFrames > 0 && m_CutsceneImages.size() >= remainingFrames)
            m_CutsceneImages.erase(m_CutsceneImages.begin(), m_CutsceneImages.begin() + remainingFrames);

        m_currShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[0];
        if (m_CutsceneImages.size() > 1)
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[1];
        else
            m_nextShot.GetComponent<Sprite>()->sprite_handle = 0;
    }

    // Update dialogue in auto–run mode.
    void CutsceneLayer::updateDialogueAuto(float dt)
    {
        updateDialogueText(dt);

        if (m_currSectionIndex < m_CutsceneDialogue.size())
        {
            const auto& dialogueBlock = m_CutsceneDialogue[m_currSectionIndex];
            if (m_currDialogueIndex < dialogueBlock.size())
            {
                std::string fullText = FLX_STRING_GET(dialogueBlock[m_currDialogueIndex]);
                size_t totalChars = fullText.size();
                size_t currentChars = static_cast<size_t>(m_dialogueTimer * m_dialogueTextRate);

                if (Input::GetKeyDown(GLFW_KEY_SPACE) || (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && enable_clickingreaction))
                {
                    if (currentChars < totalChars)
                    {
                        // Skip text animation: show full text instantly.
                        m_dialogueTimer = totalChars / m_dialogueTextRate;
                        FlexECS::Scene::StringIndex txt = FLX_STRING_NEW(fullText);
                        m_dialoguebox.GetComponent<Text>()->text = txt;
                        m_shadowdialoguebox.GetComponent<Text>()->text = txt;
                    }
                    else
                    {
                        advanceDialogue();
                    }
                }
                else // Auto–advance when text is fully revealed.
                {
                    if (currentChars >= totalChars)
                    {
                        m_dialogueHoldTimer += dt;
                        if (m_dialogueHoldTimer >= m_dialogueHoldDuration)
                            advanceDialogue();
                    }
                }
            }
        }
    }

    // Update dialogue in manual (user–controlled) mode.
    void CutsceneLayer::updateDialogueManual(float dt)
    {
        updateDialogueText(dt);

        if (Input::GetKeyDown(GLFW_KEY_SPACE) || (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && enable_clickingreaction))
        {
            if (m_currSectionIndex < m_CutsceneDialogue.size())
            {
                const auto& dialogueBlock = m_CutsceneDialogue[m_currSectionIndex];
                if (m_currDialogueIndex < dialogueBlock.size())
                {
                    std::string fullText = FLX_STRING_GET(dialogueBlock[m_currDialogueIndex]);
                    size_t totalChars = fullText.size();
                    size_t currentChars = static_cast<size_t>(m_dialogueTimer * m_dialogueTextRate);

                    if (currentChars < totalChars)
                    {
                        m_dialogueTimer = totalChars / m_dialogueTextRate;
                        FlexECS::Scene::StringIndex txt = FLX_STRING_NEW(fullText);
                        m_dialoguebox.GetComponent<Text>()->text = txt;
                        m_shadowdialoguebox.GetComponent<Text>()->text = txt;
                    }
                    else
                    {
                        advanceDialogue();
                    }
                }
                m_dialogueIsWaitingForInput = false;
            }
        }
    }

    // Update image frame timing for multi–frame sections.
    void CutsceneLayer::updateImageFrames(float dt)
    {
        if (m_frameCount > 1 && m_TransitionPhase == TransitionPhase::None)
        {
            m_ElapsedTime += dt;
            if (m_currFrameIndex < m_frameCount - 1)
            {
                if (m_ElapsedTime >= m_PerFrameDuration)
                {
                    SwapShots();
                    m_ElapsedTime = 0.0f;
                }
            }
            else // On the last frame.
            {
                if (m_ElapsedTime >= m_PerFrameDuration && m_TransitionPhase == TransitionPhase::None)
                {
                    m_TransitionPhase = TransitionPhase::PreTransition;
                    m_TransitionElapsedTime = 0.0f;
                }
            }
        }
    }

    // Update transition phases (fade–out and fade–in effects).
    void CutsceneLayer::updateTransitionPhase(float dt)
    {
        if (m_TransitionPhase == TransitionPhase::PreTransition)
        {
            m_TransitionElapsedTime += dt;
            float progress = m_TransitionElapsedTime / m_PreTransitionDuration;
            float newOpacity = FlexMath::Lerp(1.0f, 0.0f, progress);
            m_currShot.GetComponent<Sprite>()->opacity = newOpacity;

            if (m_TransitionElapsedTime >= m_PreTransitionDuration)
            {
                SwapShots();
                m_TransitionPhase = TransitionPhase::PostTransition;
                m_TransitionElapsedTime = 0.0f;
            }
        }
        else if (m_TransitionPhase == TransitionPhase::PostTransition)
        {
            m_TransitionElapsedTime += dt;
            if (m_TransitionElapsedTime >= m_PostTransitionDuration)
            {
                m_TransitionPhase = TransitionPhase::None;
                m_ElapsedTime = 0.0f;
                UpdateTimings(true); // Prepare next section.
                m_currDialogueIndex = 0;
                m_dialogueTimer = 0.0f;
                m_dialogueHoldTimer = 0.0f;
            }
        }
    }

    void CutsceneLayer::SwapShots()
    {
        // If there is only one (or zero) image left, we have reached the end.
        if (m_CutsceneImages.size() <= 1)
        {
            Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 0.5 });;
            return;
        }

        // Remove the current frame from the front of the vector.
        m_CutsceneImages.erase(m_CutsceneImages.begin());

        // Now, update the current shot to the new first element.
        m_currShot.GetComponent<Sprite>()->sprite_handle = m_nextShot.GetComponent<Sprite>()->sprite_handle;
        m_currShot.GetComponent<Sprite>()->opacity = 1.0f;
        m_currFrameIndex++;

        // Update the next shot if there is another frame.
        if (m_CutsceneImages.size() > 1)
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[1];
        else
            m_nextShot.GetComponent<Sprite>()->sprite_handle = 0;

        std::string txt = FLX_STRING_GET(m_currShot.GetComponent<Sprite>()->sprite_handle);
        std::string txt2 = FLX_STRING_GET(m_nextShot.GetComponent<Sprite>()->sprite_handle);
    }
    #pragma endregion

    #pragma region UI Animation
    void CutsceneLayer::updateInstructionAnimation(float dt)
    {
        if (!m_instructionActive)
            return;

        if (Input::GetKey(GLFW_KEY_ESCAPE) || m_instructionTimer >= m_instructionDuration)
        {
            m_instructiontxt.GetComponent<Transform>()->is_active = false;
            m_instructiontxtopacityblk.GetComponent<Transform>()->is_active = false;
            m_instructionActive = false;
            return;
        }

        m_instructionTimer += dt;
        float progress = m_instructionTimer / m_instructionDuration; // instructionDuration is a set duration
        auto* pos = m_instructiontxt.GetComponent<Position>();
        pos->position.x = FlexMath::Lerp(pos->position.x, pos->position.x+1, progress); // Just move to the right a bit
        auto* sprite = m_instructiontxtopacityblk.GetComponent<Sprite>();
        sprite->opacity = FlexMath::Lerp(0.0f, 1.0f, progress);
    }
    
    void CutsceneLayer::updateDialogueArrow(float dt)
    {
        static float m_totaltime = 0.0f;
        static float m_arrowBaseY = m_dialoguearrow.GetComponent<Position>()->position.y;
        m_totaltime += dt;
        // Only run in manual mode when the current dialogue is fully revealed.
        if (!is_autoplay && m_dialogueIsWaitingForInput)
        {
            // Increase opacity over time
            m_arrowTimer += dt;
            m_dialoguearrow.GetComponent<Sprite>()->opacity = 1.0;

            float offset = sin(m_totaltime * m_arrowOscillationFrequency) * m_arrowOscillationAmplitude;
            auto* arrowpos = m_dialoguearrow.GetComponent<Position>();
            arrowpos->position.y = m_arrowBaseY + offset;
        }
        else
        {
            m_arrowTimer = 0.0f;
            m_dialoguearrow.GetComponent<Sprite>()->opacity = 0;
            m_dialogueIsWaitingForInput = false;
        }
    }
    //void updateSkipUI(float dt);
    #pragma endregion
}
