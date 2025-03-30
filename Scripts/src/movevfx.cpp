/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// movevfx.cpp
//
// This script handles the move vfx that is needed by the game. It works by
// sending a spawn vfx message via the messaging system
// 
// AUTHORS
// [100%]  (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
using namespace FlexEngine;

// Helper function to process an effect's activation and timer update.
template<typename Func>
void ProcessEffect(bool& active, float& timer, float duration, float dt, Func updateFunc)
{
    if (active)
    {
        updateFunc(timer);  // Call the effect-specific update with the current timer.
        timer += dt;
        if (timer >= duration)
        {
            active = false;
            timer = 0.0f;
        }
    }
}

class MoveVFXSystemScript : public IScript
{
private:
    std::vector<std::pair<FlexECS::Entity, bool>> m_vfx_pool;

    // --- Effect Timers and State ---
    bool m_chromaticActive = false;
    float m_chromaticTimer = 0.0f;
    const float m_chromaticDuration = 0.5f; // seconds

    bool m_pseudoColorActive = false;
    float m_pseudoColorTimer = 0.0f;
    const float m_pseudoColorDuration = 0.2f; // seconds

    bool m_jackUltActive = false;
    float m_jackUltTimer = 0.0f;
    const float m_jackUltDuration = 1.2f; // seconds

    // --- Effect Functions ---

    // Chromatic Aberration update function. Note that it receives the elapsed time.
    void UpdateChromaticEffect(float elapsedTime, float totalDuration)
    {
        // Clamp elapsedTime if needed.
        float clampedTime = (elapsedTime > totalDuration) ? totalDuration : elapsedTime;

        // Calculate half duration for ramp up and down.
        float halfDuration = totalDuration / 2.0f;
        constexpr float targetIntensity = 0.8f;

        // Calculate the current intensity based on elapsed time.
        float currentIntensity = (clampedTime <= halfDuration)
            ? FlexMath::Lerp(0.0f, targetIntensity, clampedTime / halfDuration)
            : FlexMath::Lerp(targetIntensity, 0.0f, (clampedTime - halfDuration) / halfDuration);

        // Process each entity with a PostProcessingMarker and Transform in the active scene.
        auto activeScene = FlexECS::Scene::GetActiveScene();
        if (activeScene)
        {
            for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
            {
                // Ensure the entity has the chromatic aberration component.
                if (!element.HasComponent<PostProcessingChromaticAbberation>())
                {
                    element.AddComponent<PostProcessingChromaticAbberation>({});
                }

                auto aberration = element.GetComponent<PostProcessingChromaticAbberation>();
                aberration->intensity = currentIntensity;
                aberration->edgeRadius = Vector2(0.02f, 0.02f);
                aberration->edgeSoftness = Vector2(0.3f, 0.3f);
                // Enable chromatic aberration on the marker.
                element.GetComponent<PostProcessingMarker>()->enableChromaticAberration = true;
            }
        }
    }

    // Stub for pseudo color distortion effect.
    void UpdatePseudoColorEffect(float elapsedTime, float totalDuration)
    {
        bool enableEffect = elapsedTime < (totalDuration-0.05f); // True if still in effect duration

        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>())
        {
            if (!entity.GetComponent<Transform>()->is_active)
                continue;

            // Ensure the entity has a Color Grading component
            if (!entity.HasComponent<PostProcessingColorGrading>())
            {
                entity.AddComponent<PostProcessingColorGrading>({});
            }

            auto colorGrading = entity.GetComponent<PostProcessingColorGrading>();
            if (!colorGrading)
                continue;

            if (enableEffect)
            {
                // (Bright Distortion)
                colorGrading->saturation = 0.3f; 
                colorGrading->contrast = -0.9f;   
                colorGrading->brightness = 0.2f; 
            }
            else
            {
                // **Revert to normal instantly**
                colorGrading->saturation = 3.0f;
                colorGrading->contrast = 1.5f;
                colorGrading->brightness = 0.8f;
            }

            // Enable the effect marker
            entity.GetComponent<PostProcessingMarker>()->enableColorGrading = enableEffect;
        }
    }

    // Stub for Jack Ult effects.
    void UpdateJackUltEffect(float elapsedTime, float totalDuration)
    {
        float progress = FlexMath::Clamp(elapsedTime / totalDuration, 0.0f, 1.0f); // Normalize progress (0 to 1)

        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>())
        {
            if (!entity.GetComponent<Transform>()->is_active)
                continue;

            // Ensure the entity has a Color Grading component
            if (!entity.HasComponent<PostProcessingColorGrading>())
            {
                entity.AddComponent<PostProcessingColorGrading>({});
            }

            auto colorGrading = entity.GetComponent<PostProcessingColorGrading>();
            if (!colorGrading)
                continue;

            // **Lerp to target values**
            colorGrading->brightness = 0.0f;
            colorGrading->saturation = 1.0f;
            colorGrading->contrast = FlexMath::Lerp(1.0f, 3.7f, progress);

            // Enable the effect marker
            entity.GetComponent<PostProcessingMarker>()->enableColorGrading = true;
        }

        // **Trigger pseudo color distortion at the start of the effect**
        if (elapsedTime >= totalDuration - 0.3f) // Ensure it only triggers once
        {
            Application::MessagingSystem::Send("ActivatePseudoColorDistortion", true);
        }
    }

public:
    MoveVFXSystemScript()
    {
        ScriptRegistry::RegisterScript(this);
    }

    std::string GetName() const override
    {
        return "MoveVFXSystem";
    }

    void Update() override
    {
        float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // --- VFX Pool Initialization and Spawn ---
        if (Application::MessagingSystem::Receive<bool>("Initialize VFX"))
        {
            m_vfx_pool.resize(7);
            for (size_t i = 0; i < 7; i++)
            {
                m_vfx_pool[i].first = FlexECS::Scene::CreateEntity("MoveVFX" + std::to_string(i));
                m_vfx_pool[i].first.AddComponent<Transform>({});
                m_vfx_pool[i].first.AddComponent<Position>({ {-50000, -50000, 1} });
                m_vfx_pool[i].first.AddComponent<Rotation>({});
                m_vfx_pool[i].first.AddComponent<Scale>({ Vector3(5.0f, 5.0f, 5.0f) });
                m_vfx_pool[i].first.AddComponent<Sprite>({});
                m_vfx_pool[i].first.AddComponent<Animator>({});
                m_vfx_pool[i].first.AddComponent<ZIndex>({ 1 });

                auto& animator = *m_vfx_pool[i].first.GetComponent<Animator>();
                animator.should_play = false;
                animator.is_looping = false;

                m_vfx_pool[i].second = false;
            }
        }
        //                                                                  list of targets               //vfx key  //positionoffset  //scale
        auto vfx_targets = Application::MessagingSystem::Receive<std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>>("Spawn VFX");
        if (std::get<0>(vfx_targets).size() > 0)
        {
            for (size_t i = 0; i < std::get<0>(vfx_targets).size(); i++)
            {
                auto target_entity = std::get<0>(vfx_targets)[i];
                for (auto& e : m_vfx_pool)
                {
                    if (e.second == true)
                        continue;

                    e.first.GetComponent<Position>()->position = target_entity.GetComponent<Position>()->position;
                    e.first.GetComponent<Position>()->position += std::get<2>(vfx_targets);
                    e.first.GetComponent<Scale>()->scale = std::get<3>(vfx_targets);

                    e.first.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(std::get<1>(vfx_targets));
                    e.first.GetComponent<Animator>()->should_play = true;
                    e.first.GetComponent<Animator>()->is_looping = false;
                    e.first.GetComponent<Animator>()->return_to_default = true;
                    e.first.GetComponent<Animator>()->frame_time = 0.f;
                    e.first.GetComponent<Animator>()->current_frame = 0;

                    e.second = true;
                    break;
                }
            }
        }

        //>>>>TAKE NOTE:<<<<<<
        //>>>>THIS IS A HACK<<<<< BECAUSE I DONT WANNA WAKE PEOPLE UP AT 3:08AM
        // Partially a graphics problem. After animation finishes, if it doesnt have a default animation,
        // A black box is shown.
        // Thus, we just move the black box far away lmao
        for (auto& e : m_vfx_pool)
        {
            if (!e.second)
                continue;

            Animator& animator = *e.first.GetComponent<Animator>();
            if ((animator.current_frame >= animator.total_frames - 1) &&
                (animator.total_frames != 0) &&
                (animator.current_frame_time - animator.frame_time < 0.1f))
            {
                e.first.GetComponent<Position>()->position = { -50000, -50000, 0 };
                e.second = false;
            }
        }

        // --- Effects Update ---

        // Chromatic Alteration:
        bool startchromaticalteration = Application::MessagingSystem::Receive<bool>("ActivateChromaticAlteration");
        if (startchromaticalteration)
        {
            m_chromaticActive = true;
        }
        ProcessEffect(m_chromaticActive, m_chromaticTimer, m_chromaticDuration, dt,
          [this](float elapsed)
        {
            UpdateChromaticEffect(elapsed, m_chromaticDuration);
        });

        // Pseudo Color Distortion:
        bool startDistortion = Application::MessagingSystem::Receive<bool>("ActivatePseudoColorDistortion");
        if (startDistortion)
        {
            m_pseudoColorActive = true;
        }
        ProcessEffect(m_pseudoColorActive, m_pseudoColorTimer, m_pseudoColorDuration, dt,
          [this](float elapsed) {
            UpdatePseudoColorEffect(elapsed, m_pseudoColorDuration);
        });

        // Jack Ult Effects:
        bool startJackUltEffects = Application::MessagingSystem::Receive<bool>("ActivateJackUlt");
        if (startJackUltEffects)
        {
            m_jackUltActive = true;
        }
        ProcessEffect(m_jackUltActive, m_jackUltTimer, m_jackUltDuration, dt,
          [this](float elapsed) {
            UpdateJackUltEffect(elapsed, m_jackUltDuration);
        });
    }

    void OnMouseEnter() override {}
    void OnMouseStay() override {}
    void OnMouseExit() override {}
};

// Static instance to ensure registration
static MoveVFXSystemScript MoveVFXSystem;
