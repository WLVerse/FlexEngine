/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// movevfx.cpp
//
// Implements MoveVFXSystemScript to manage movement-related VFX, including
// spawning pooled effect entities, updating effect timers, and applying
// chromatic aberration, pseudo-color distortion, and Jack Ult post-processing
// effects via the messaging system.
//
// AUTHORS
// [50%] Rocky (rocky.sutarius@digipen.edu)
//   - Main Author
// [50%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Sub Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
using namespace FlexEngine;

// Function: ProcessEffect
// Description: If 'active' is true, calls updateFunc(timer), increments 'timer'
//              by dt, and deactivates/resets when timer >= duration.
template<typename Func>
void ProcessEffect(bool& active, float& timer, float duration, float dt, Func updateFunc)
{
    if (active)
    {
        updateFunc(timer);
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
    bool  m_chromaticActive = false;
    float m_chromaticTimer = 0.0f;
    const float m_chromaticDuration = 0.5f; // seconds

    bool  m_pseudoColorActive = false;
    float m_pseudoColorTimer = 0.0f;
    const float m_pseudoColorDuration = 0.2f; // seconds

    bool  m_jackUltActive = false;
    float m_jackUltTimer = 0.0f;
    const float m_jackUltDuration = 1.2f; // seconds

    // Function: UpdateChromaticEffect
    // Description: Applies chromatic aberration effect over 'totalDuration'
    //              based on 'elapsedTime', updating intensity and enabling it.
    void UpdateChromaticEffect(float elapsedTime, float totalDuration)
    {
        float clampedTime = (elapsedTime > totalDuration) ? totalDuration : elapsedTime;
        float halfDuration = totalDuration / 2.0f;
        constexpr float targetIntensity = 0.8f;
        float currentIntensity = (clampedTime <= halfDuration)
            ? FlexMath::Lerp(0.0f, targetIntensity, clampedTime / halfDuration)
            : FlexMath::Lerp(targetIntensity, 0.0f, (clampedTime - halfDuration) / halfDuration);

        auto activeScene = FlexECS::Scene::GetActiveScene();
        if (activeScene)
        {
            for (auto& element : activeScene->CachedQuery<PostProcessingMarker, Transform>())
            {
                if (!element.HasComponent<PostProcessingChromaticAbberation>())
                    element.AddComponent<PostProcessingChromaticAbberation>({});

                auto aberration = element.GetComponent<PostProcessingChromaticAbberation>();
                aberration->intensity = currentIntensity;
                aberration->edgeRadius = Vector2(0.02f, 0.02f);
                aberration->edgeSoftness = Vector2(0.3f, 0.3f);
                element.GetComponent<PostProcessingMarker>()->enableChromaticAberration = true;
            }
        }
    }

    // Function: UpdatePseudoColorEffect
    // Description: Toggles pseudo-color distortion for duration, then reverts
    //              saturation, contrast, and brightness instantly.
    void UpdatePseudoColorEffect(float elapsedTime, float totalDuration)
    {
        bool enableEffect = elapsedTime < (totalDuration - 0.05f);
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>())
        {
            if (!entity.GetComponent<Transform>()->is_active) continue;
            if (!entity.HasComponent<PostProcessingColorGrading>())
                entity.AddComponent<PostProcessingColorGrading>({});

            auto colorGrading = entity.GetComponent<PostProcessingColorGrading>();
            if (!colorGrading) continue;

            if (enableEffect)
            {
                colorGrading->saturation = 0.3f;
                colorGrading->contrast = -0.9f;
                colorGrading->brightness = 0.2f;
            }
            else
            {
                colorGrading->saturation = 3.0f;
                colorGrading->contrast = 1.5f;
                colorGrading->brightness = 0.8f;
            }
            entity.GetComponent<PostProcessingMarker>()->enableColorGrading = enableEffect;
        }
    }

    // Function: UpdateJackUltEffect
    // Description: Applies Jack Ult color grading effect over duration, and
    //              triggers pseudo-color distortion near the end.
    void UpdateJackUltEffect(float elapsedTime, float totalDuration)
    {
        float progress = FlexMath::Clamp(elapsedTime / totalDuration, 0.0f, 1.0f);
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>())
        {
            if (!entity.GetComponent<Transform>()->is_active) continue;
            if (!entity.HasComponent<PostProcessingColorGrading>())
                entity.AddComponent<PostProcessingColorGrading>({});

            auto colorGrading = entity.GetComponent<PostProcessingColorGrading>();
            if (!colorGrading) continue;

            colorGrading->brightness = 0.0f;
            colorGrading->saturation = 1.0f;
            colorGrading->contrast = FlexMath::Lerp(1.0f, 3.7f, progress);
            entity.GetComponent<PostProcessingMarker>()->enableColorGrading = true;
        }

        if (elapsedTime >= totalDuration - 0.3f)
            Application::MessagingSystem::Send("ActivatePseudoColorDistortion", true);
    }

public:
    // Function: MoveVFXSystemScript
    // Description: Registers this script instance with the engine on creation.
    MoveVFXSystemScript()
    {
        ScriptRegistry::RegisterScript(this);
    }

    // Function: GetName
    // Description: Returns the unique name of this script.
    std::string GetName() const override
    {
        return "MoveVFXSystem";
    }

    // Function: Update
    // Description: Main per-frame update: initializes pool, handles spawn messages,
    //              moves finished VFX off-screen, and processes effect timers.
    void Update() override
    {
        float dt = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // Pool initialization
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
                m_vfx_pool[i].first.AddComponent<ZIndex>({ 998 });

                auto& animator = *m_vfx_pool[i].first.GetComponent<Animator>();
                animator.should_play = false;
                animator.is_looping = false;
                m_vfx_pool[i].second = false;
            }
        }

        // Spawn VFX
        auto vfx_targets = Application::MessagingSystem::Receive<
            std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
        >("Spawn VFX");
        if (!std::get<0>(vfx_targets).empty())
        {
            for (size_t i = 0; i < std::get<0>(vfx_targets).size(); i++)
            {
                auto target_entity = std::get<0>(vfx_targets)[i];
                for (auto& e : m_vfx_pool)
                {
                    if (e.second) continue;

                    e.first.GetComponent<Position>()->position =
                        target_entity.GetComponent<Position>()->position + std::get<2>(vfx_targets);
                    e.first.GetComponent<Scale>()->scale = std::get<3>(vfx_targets);

                    auto anim = e.first.GetComponent<Animator>();
                    anim->spritesheet_handle = FLX_STRING_NEW(std::get<1>(vfx_targets));
                    anim->should_play = true;
                    anim->is_looping = false;
                    anim->return_to_default = true;
                    anim->frame_time = 0.0f;
                    anim->current_frame = 0;

                    e.second = true;
                    break;
                }
            }
        }

        // Cleanup finished VFX
        for (auto& e : m_vfx_pool)
        {
            if (!e.second) continue;
            Animator& animator = *e.first.GetComponent<Animator>();
            if ((animator.current_frame >= animator.total_frames - 1) &&
                (animator.total_frames != 0) &&
                (animator.current_frame_time - animator.frame_time < 0.1f))
            {
                e.first.GetComponent<Position>()->position = { -50000, -50000, 0 };
                e.second = false;
            }
        }

        // Chromatic Aberration
        if (Application::MessagingSystem::Receive<bool>("ActivateChromaticAlteration"))
            m_chromaticActive = true;
        ProcessEffect(m_chromaticActive, m_chromaticTimer, m_chromaticDuration, dt,
                      [this](float elapsed) { UpdateChromaticEffect(elapsed, m_chromaticDuration); });

        // Pseudo-Color Distortion
        if (Application::MessagingSystem::Receive<bool>("ActivatePseudoColorDistortion"))
            m_pseudoColorActive = true;
        ProcessEffect(m_pseudoColorActive, m_pseudoColorTimer, m_pseudoColorDuration, dt,
                      [this](float elapsed) { UpdatePseudoColorEffect(elapsed, m_pseudoColorDuration); });

        // Jack Ult Effects
        if (Application::MessagingSystem::Receive<bool>("ActivateJackUlt"))
            m_jackUltActive = true;
        ProcessEffect(m_jackUltActive, m_jackUltTimer, m_jackUltDuration, dt,
                      [this](float elapsed) { UpdateJackUltEffect(elapsed, m_jackUltDuration); });
    }

    // Function: OnMouseEnter
    // Description: Mouse enter event stub (unused).
    void OnMouseEnter() override {}

    // Function: OnMouseStay
    // Description: Mouse stay event stub (unused).
    void OnMouseStay() override {}

    // Function: OnMouseExit
    // Description: Mouse exit event stub (unused).
    void OnMouseExit() override {}
};

// Static instance to ensure registration
static MoveVFXSystemScript MoveVFXSystem;
