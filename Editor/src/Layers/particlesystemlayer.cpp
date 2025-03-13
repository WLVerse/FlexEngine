/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// particlesystemlayer.cpp
//
// Implements the ParticleSystemLayer class, which updates all particle emitters,
// spawns and pools particles, and controls particle simulation and interpolation
// of particle properties during their lifecycle.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "Layers.h"
#include "editor.h"

namespace Editor
{

    // Called when the layer is first attached to the application.
    void ParticleSystemLayer::OnAttach()
    {
        // Initialize resources if needed (e.g., particle textures or materials)
    }

    // Called when the layer is removed from the application.
    void ParticleSystemLayer::OnDetach()
    {
        // Clear particle references to prevent dangling handles
        m_ParticleEntities.clear();
    }

    // Called every frame to update particle systems and particles.
    void ParticleSystemLayer::Update()
    {
        float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // --- EMITTER UPDATE: Process all ParticleSystem (emitter) components ---
        for (auto& emitterEntity : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem>())
        {
            auto transform = emitterEntity.GetComponent<Transform>();
            if (!transform || !transform->is_active)
                continue;

            auto emitter = emitterEntity.GetComponent<ParticleSystem>();

            // Update the emitter's timer
            emitter->duration -= deltaTime;
            if (!emitter->is_looping && emitter->duration <= 0.0f)
            {
                transform->is_active = false;
                continue;
            }

            // Handle emission rate over time
            emitter->emissionAccumulator += deltaTime * emitter->particleEmissionRate.rate_over_time;
            int particlesToEmit = static_cast<int>(emitter->emissionAccumulator);
            emitter->emissionAccumulator -= particlesToEmit;

            for (int i = 0; i < particlesToEmit; ++i)
            {
                SpawnParticles(emitterEntity);
            }
        }

        // --- PARTICLE UPDATE: Process all Particle components ---
        std::vector<FlexECS::EntityID> m_entities_to_delete;

        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem::Particle>())
        {
            auto particle = elem.GetComponent<ParticleSystem::Particle>();

            // Update lifetime
            particle->currentLifetime -= deltaTime;
            if (particle->currentLifetime < -20.0f)
            {
                m_entities_to_delete.push_back(elem.Get());
                continue;
            }
            else if (particle->currentLifetime <= 0.0f)
            {
                elem.GetComponent<Transform>()->is_active = false;
                continue;
            }

            // Update position based on velocity
            elem.GetComponent<Position>()->position += static_cast<Vector3>(elem.GetComponent<Rigidbody>()->velocity) * deltaTime;

            // Compute normalized lifetime for interpolation (0 = end, 1 = start)
            float normalizedTime = 1.0f - (particle->currentLifetime / particle->totalLifetime);

            // Interpolate particle properties
            particle->currentSpeed = FlexMath::Lerp(particle->start_speed, particle->end_speed, normalizedTime);
            particle->currentSize = FlexMath::Lerp(particle->start_size, particle->end_size, normalizedTime);
            particle->currentColor = Lerp(particle->start_color, particle->end_color, normalizedTime);

            // Update visual size
            elem.GetComponent<Scale>()->scale = Vector3(particle->currentSize, particle->currentSize, particle->currentSize);
        }

        // Destroy all fully expired particles
        auto scene = FlexECS::Scene::GetActiveScene();
        for (auto entity : m_entities_to_delete)
        {
            scene->DestroyEntity(entity);
            Editor::GetInstance().GetSystem<SelectionSystem>()->DeselectEntity(entity);
        }
    }

    /// Attempts to spawn a particle by reusing an inactive one, or creates a new particle entity.
    void ParticleSystemLayer::SpawnParticles(FlexECS::Entity& emitterEntity)
    {
        FlexECS::Entity particleEntity;
        bool foundInactive = false;

        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem::Particle>())
        {
            if (!elem.GetComponent<Transform>()->is_active)
            {
                particleEntity = elem;
                foundInactive = true;
                break;
            }
        }

        if (foundInactive)
        {
            PoolOldParticles(emitterEntity, particleEntity);
        }
        else
        {
            CreateNewParticles(emitterEntity);
        }
    }

    /// Creates a brand-new particle entity and initializes its components.
    void ParticleSystemLayer::CreateNewParticles(FlexECS::Entity& emitterEntity)
    {
        auto emitter = emitterEntity.GetComponent<ParticleSystem>();

        FlexECS::Entity particleEntity = FlexECS::Scene::GetActiveScene()->CreateEntity();
        particleEntity.AddComponent<Position>({ emitterEntity.GetComponent<Position>()->position });
        particleEntity.AddComponent<Rotation>({ Vector3{0,0,0} });
        particleEntity.AddComponent<Scale>({ {emitter->start_size, emitter->start_size, emitter->start_size} });
        particleEntity.AddComponent<Transform>({});
        particleEntity.AddComponent<Sprite>({ emitter->particlesprite_handle, -1 });

        Vector3 direction = CalculateInitialVelocity(emitter->particleEmissionShapeIndex);
        if (auto rotationComp = emitterEntity.GetComponent<Rotation>())
        {
            direction = RotateVector(direction, rotationComp->rotation);
        }

        Vector3 velocity = direction * emitter->start_speed;
        particleEntity.AddComponent<Rigidbody>({ velocity, emitter->is_static });

        if (emitter->is_collidable)
        {
            particleEntity.AddComponent<BoundingBox2D>({});
            particleEntity.AddComponent<AABB>({});
        }

        // Initialize particle state
        ParticleSystem::Particle particle;
        particle.totalLifetime = emitter->lifetime;
        particle.currentLifetime = emitter->lifetime;
        particle.start_speed = emitter->start_speed;
        particle.end_speed = emitter->end_speed;
        particle.start_size = emitter->start_size;
        particle.end_size = emitter->end_size;
        particle.start_color = emitter->start_color;
        particle.end_color = emitter->end_color;
        particle.currentSpeed = particle.start_speed;
        particle.currentSize = particle.start_size;
        particle.currentColor = particle.start_color;

        particleEntity.AddComponent<ParticleSystem::Particle>(particle);
    }

    /// Reuses and reinitializes an inactive particle to avoid recreating entities.
    void ParticleSystemLayer::PoolOldParticles(FlexECS::Entity& emitterEntity, FlexECS::Entity& particleEntity)
    {
        auto emitter = emitterEntity.GetComponent<ParticleSystem>();

        particleEntity.GetComponent<Position>()->position = emitterEntity.GetComponent<Position>()->position;
        particleEntity.GetComponent<Rotation>()->rotation = Vector3{ 0, 0, 0 };
        particleEntity.GetComponent<Scale>()->scale = Vector3(emitter->start_size);
        particleEntity.GetComponent<Transform>()->is_active = true;
        particleEntity.GetComponent<Sprite>()->sprite_handle = emitter->particlesprite_handle;

        Vector3 direction = CalculateInitialVelocity(emitter->particleEmissionShapeIndex);
        if (auto rotationComp = emitterEntity.GetComponent<Rotation>())
        {
            direction = RotateVector(direction, rotationComp->rotation);
        }

        Vector3 velocity = direction * emitter->start_speed;
        if (particleEntity.HasComponent<Rigidbody>())
        {
            particleEntity.GetComponent<Rigidbody>()->velocity = velocity;
            particleEntity.GetComponent<Rigidbody>()->is_static = emitter->is_static;
        }
        else
        {
            particleEntity.AddComponent<Rigidbody>({ velocity, emitter->is_static });
        }

        // Refresh particle state
        auto particle = particleEntity.GetComponent<ParticleSystem::Particle>();
        particle->totalLifetime = emitter->lifetime;
        particle->currentLifetime = emitter->lifetime;
        particle->start_speed = emitter->start_speed;
        particle->end_speed = emitter->end_speed;
        particle->start_size = emitter->start_size;
        particle->end_size = emitter->end_size;
        particle->start_color = emitter->start_color;
        particle->end_color = emitter->end_color;
        particle->currentSpeed = emitter->start_speed;
        particle->currentSize = emitter->start_size;
        particle->currentColor = emitter->start_color;
    }

    /// Calculates a normalized initial velocity vector based on emission shape.
    Vector3 ParticleSystemLayer::CalculateInitialVelocity(const int shape)
    {
        switch (shape)
        {
        case ParticleEmitShape::Sphere:
            return RandomUnitVector();

        case ParticleEmitShape::Hemisphere:
        {
            Vector3 v = RandomUnitVector();
            if (v.y < 0.0f) v.y = -v.y;
            return v.Normalize();
        }

        case ParticleEmitShape::Cone:
            return RandomUnitVectorCone(30.0f);

        case ParticleEmitShape::Box:
            return RandomUnitVectorCube();

        default:
            return Vector3(0.0f, 1.0f, 0.0f); // Default upward
        }
    }

} // namespace Editor
