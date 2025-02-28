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
        loadCutscene(FLX_STRING_NEW(R"(/cutscenes/OpeningCutscene.flxdialogue)"),
                     FLX_STRING_NEW(R"(/cutscenes/OpeningCutscene.flxcutscene)"));

        m_currFrameIndex = 0;
        m_currSectionIndex = 0;

        // Create the current shot entity.
        // Intentionally setting sprite handle to 0.
        m_currShot = FlexECS::Scene::GetActiveScene()->CreateEntity("Current Cinematic Shot");
        m_currShot.AddComponent<Position>({});
        m_currShot.AddComponent<Rotation>({});
        m_currShot.AddComponent<Scale>({ Vector3::One });
        m_currShot.AddComponent<Sprite>({ 0 });
        m_currShot.AddComponent<Transform>({ Matrix4x4::Identity, true });
        m_currShot.AddComponent<ZIndex>({ 10 });

        // Create the next shot entity and assign the image if available.
        m_nextShot = FlexECS::Scene::GetActiveScene()->CreateEntity("Next Cinematic Shot");
        m_nextShot.AddComponent<Position>({});
        m_nextShot.AddComponent<Rotation>({});
        m_nextShot.AddComponent<Scale>({ Vector3::One });
        if (m_currFrameIndex + 1 < m_CutsceneImages.size())
            m_nextShot.AddComponent<Sprite>({ m_CutsceneImages[m_currFrameIndex] });
        else
            m_nextShot.AddComponent<Sprite>({ 0 });
        m_nextShot.AddComponent<Transform>({ Matrix4x4::Identity, true });
        m_nextShot.AddComponent<ZIndex>({ 9 });

        m_dialoguebox = FlexECS::Scene::GetActiveScene()->CreateEntity("Normal Dialogue Box");
        m_dialoguebox.AddComponent<Position>({ Vector3(0,-390.0f,0) });
        m_dialoguebox.AddComponent<Rotation>({});
        m_dialoguebox.AddComponent<Scale>({ Vector3::One });
        m_dialoguebox.AddComponent<Transform>({});
        m_dialoguebox.AddComponent<ZIndex>({ 1000 });
        m_dialoguebox.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
          FLX_STRING_NEW(
            R"("")"
          ),
          Vector3(1.0f, 1.0, 1.0f),
          { Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Middle },
            Vector2(Application::GetCurrentWindow()->GetWidth() * 0.8f, 70.0f)
        });

        m_shadowdialoguebox = FlexECS::Scene::GetActiveScene()->CreateEntity("Shadow Dialogue Box");
        m_shadowdialoguebox.AddComponent<Position>({ Vector3(1.5f,-393.0f,0) });
        m_shadowdialoguebox.AddComponent<Rotation>({});
        m_shadowdialoguebox.AddComponent<Scale>({ Vector3::One });
        m_shadowdialoguebox.AddComponent<Transform>({});
        m_shadowdialoguebox.AddComponent<ZIndex>({ 999 });
        m_shadowdialoguebox.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
          FLX_STRING_NEW(
            R"("")"
          ),
          Vector3(0.05f, 0.05f, 0.05f),
          { Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Middle },
            Vector2(Application::GetCurrentWindow()->GetWidth() * 0.8f, 70.0f)
        });
        auto& font = FLX_ASSET_GET(Asset::Font, R"(/fonts/Electrolize/Electrolize-Regular.ttf)");
        font.SetFontSize(30);

        StartCutscene();
    }

    void CutsceneLayer::OnDetach()
    {
        // Cleanup code, if needed.
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

        if (!m_CutsceneActive)
            return;

        // Process global input (restart, escape).
        processGlobalInput();

        float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

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
            StopCutscene();
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
            StopCutscene();
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
                    charsToShow = totalChars;
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

                if (Input::GetKeyDown(GLFW_KEY_SPACE) || Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
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

        if (Input::GetKeyDown(GLFW_KEY_SPACE) || Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
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
            StopCutscene();
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
}
