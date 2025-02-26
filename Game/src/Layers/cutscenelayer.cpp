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
    void CutsceneLayer::loadCutscene(FlexECS::Scene::StringIndex dialogue_file, FlexECS::Scene::StringIndex cutscene_file)
    {
        // Retrieve the dialogue and cutscene assets.
        auto& asset_dialogue = FLX_ASSET_GET(Asset::Dialogue, FLX_STRING_GET(dialogue_file));
        auto& asset_cutscene = FLX_ASSET_GET(Asset::Cutscene, FLX_STRING_GET(cutscene_file));
        m_currDialogueFile = dialogue_file;
        m_currCutsceneFile = cutscene_file;

        // Clear the current cutscene images vector.
        m_CutsceneImages.clear();

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
        }

        // Update the timings of cutscene
        UpdateTimings(false);
    }

    void CutsceneLayer::UpdateTimings(bool toNextSection)
    {
        // Retrieve the dialogue and cutscene assets.
        auto& dialogueAsset = FLX_ASSET_GET(Asset::Dialogue, FLX_STRING_GET(m_currDialogueFile));
        auto& cutsceneAsset = FLX_ASSET_GET(Asset::Cutscene, FLX_STRING_GET(m_currCutsceneFile));

        // If moving to the next section, update section and reset the current frame index.
        if (toNextSection)
        {
            // Remove the images for the finished section.
            // m_currSectionFrameCount is assumed to be set from the previous section.
            if (m_frameCount > 0 && m_CutsceneImages.size() >= static_cast<size_t>(m_frameCount))
            {
                m_CutsceneImages.erase(m_CutsceneImages.begin(),
                                       m_CutsceneImages.begin() + m_frameCount);
            }
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
        // Reset indices and timers.
        m_currFrameIndex = 0;
        m_ElapsedTime = 0.0f;
        m_TransitionElapsedTime = 0.0f;
        m_TransitionPhase = TransitionPhase::None;

        // Set up the current shot with the first image.
        m_currShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currFrameIndex];
        // Set up the next shot with the second image if available.
        if (m_currFrameIndex + 1 < m_CutsceneImages.size())
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currFrameIndex + 1];
        else
            m_nextShot.GetComponent<Sprite>()->sprite_handle = 0;

        m_CutsceneActive = true;
    }

    void CutsceneLayer::Update()
    {
        if (m_currShot == FlexECS::Entity::Null || m_nextShot == FlexECS::Entity::Null)
        {
            Log::Debug("Cutscene Shots have been deleted. Please do not delete them.");
            return;
        }


        if (!m_CutsceneActive)
            return;

        // Debug output for transition phases.
        #if 0
        if (m_TransitionPhase == TransitionPhase::PreTransition)
            std::cout << "PRE" << std::endl;
        if (m_TransitionPhase == TransitionPhase::PostTransition)
            std::cout << "POST" << std::endl;
        #endif

        // --- Input Handling for skipping ---
        if (Input::GetKeyDown(GLFW_KEY_SPACE))
        {
            // If already in a transition, swap immediately.
            if (m_TransitionPhase == TransitionPhase::PreTransition)
                SwapShots();
            // Force the pre-transition phase.
            m_TransitionPhase = TransitionPhase::PreTransition;
            m_TransitionElapsedTime = 0.0f;
        }
        else if (Input::GetKey(GLFW_KEY_ESCAPE))
        {
            StopCutscene();
            return;
        }

        // Get delta time.
        float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // --- Phase-Based Update ---
        if (m_TransitionPhase == TransitionPhase::None)
        {
            m_ElapsedTime += dt;

            // If we're not on the last frame of the current section...
            if (m_currFrameIndex < m_frameCount - 1)
            {
                if (m_ElapsedTime >= m_PerFrameDuration)
                {
                    // Instant frame swap (no fade) for intermediate frames.
                    SwapShots();
                    m_ElapsedTime = 0.0f;
                }
            }
            else // We are on the last frame of this section.
            {
                if (m_ElapsedTime >= m_PerFrameDuration)
                {
                    // Begin the transition effect only after the final frame has been displayed.
                    m_TransitionPhase = TransitionPhase::PreTransition;
                    m_TransitionElapsedTime = 0.0f;
                }
            }
        }
        else if (m_TransitionPhase == TransitionPhase::PreTransition)
        {
            // Fade out current shot.
            m_TransitionElapsedTime += dt;
            float progress = m_TransitionElapsedTime / m_PreTransitionDuration;
            float newOpacity = FlexMath::Lerp(1.0f, 0.0f, progress);
            m_currShot.GetComponent<Sprite>()->opacity = newOpacity;

            if (m_TransitionElapsedTime >= m_PreTransitionDuration)
            {
                // When fade-out completes, swap (for the final frame) and then start fade-in.
                SwapShots();
                m_TransitionPhase = TransitionPhase::PostTransition;
                m_TransitionElapsedTime = 0.0f;
            }
        }
        else if (m_TransitionPhase == TransitionPhase::PostTransition)
        {
            // Fade in new shot.
            m_TransitionElapsedTime += dt;
            float progress = m_TransitionElapsedTime / m_PostTransitionDuration;
            float newOpacity = FlexMath::Lerp(0.0f, 1.0f, progress);
            m_currShot.GetComponent<Sprite>()->opacity = newOpacity;

            if (m_TransitionElapsedTime >= m_PostTransitionDuration)
            {
                // Transition complete. Reset timers and update timings for the next section.
                m_TransitionPhase = TransitionPhase::None;
                m_ElapsedTime = 0.0f;
                UpdateTimings(true);
            }
        }
    }

    void CutsceneLayer::SwapShots()
    {
        if (m_currFrameIndex >= m_CutsceneImages.size() + 1)
        {
            StopCutscene();
            return;
        }

        // The next shot becomes the current shot.
        m_currShot.GetComponent<Sprite>()->sprite_handle = m_nextShot.GetComponent<Sprite>()->sprite_handle;
        m_currShot.GetComponent<Sprite>()->opacity = 1.0f;
        m_currFrameIndex++;

        // Prepare the next shot with the subsequent image, if available.
        if (m_currFrameIndex < m_CutsceneImages.size())
        {
            m_nextShot.GetComponent<Sprite>()->sprite_handle = m_CutsceneImages[m_currFrameIndex];
        }
        else
        {
            m_nextShot.GetComponent<Sprite>()->sprite_handle = 0;
        }
    }
}
