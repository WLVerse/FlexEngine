/*!************************************************************************
 * PARTICLE SYSTEM LAYER [https://wlverse.web.app]
 * particlesystemlayer.cpp - EDITOR
 *
 * This file implements the ParticleSystemLayer class, which manages particle
 * emitters and individual particle updates in the scene. The layer handles
 * emitter updates (such as emission rates, durations, and looping behavior),
 * particle spawning (using either a pool of inactive particles or creating new ones),
 * and particle updates (lifetime, velocity, size, color, etc.). It also performs cleanup
 * of expired particles from the scene.
 *
 * Key functionalities include:
 * - Updating emitter components based on delta time.
 * - Emitting particles based on the emission rate.
 * - Updating individual particle properties such as position, lifetime, speed,
 *   size, and color over time.
 * - Spawning new particles or reusing inactive ones.
 *
 * The design emphasizes efficient particle management by pooling inactive particles
 * and performing necessary cleanup to avoid memory leaks.
 *
 * AUTHORS
 * [100%] Soh Wei Jie (weijie_soh.c@digipen.edu)
 *   - Main Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/

#include "Layers.h"
#include "editor.h"

namespace Editor
{

    /*!
     * \brief Called when the ParticleSystemLayer is attached.
     *
     * Initializes any required resources for particle systems (e.g., loading textures).
     */
    void ParticleSystemLayer::OnAttach()
    {
        // Initialize resources if needed (e.g., load textures)
    }

    /*!
     * \brief Called when the ParticleSystemLayer is detached.
     *
     * Performs cleanup by clearing all particle entities.
     */
    void ParticleSystemLayer::OnDetach()
    {
        // Cleanup: destroy particle entities if necessary
        m_ParticleEntities.clear();
    }

    /*!
     * \brief Updates emitter and particle components each frame.
     *
     * This function performs the following operations:
     * - Updates emitter durations and handles emission based on rate over time.
     * - Spawns particles if necessary (using either a pooled inactive particle or
     *   creating a new one).
     * - Updates each particle's lifetime, position, and interpolated properties
     *   (speed, size, color) based on the normalized lifetime.
     * - Removes expired particles from the scene.
     */
    void ParticleSystemLayer::Update()
    {
        float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        // --- EMITTER UPDATE: Process all ParticleSystem (emitter) components ---
        for (auto& emitterEntity : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem>())
        {
            // Process only active emitters (e.g., check if the associated Transform is active)
            auto transform = emitterEntity.GetComponent<Transform>();
            if (!transform || !transform->is_active)
                continue;

            // Access the ParticleSystem (emitter) component.
            auto emitter = emitterEntity.GetComponent<ParticleSystem>();

            // Update the emitter’s duration timer.
            emitter->duration -= deltaTime;
            if (!emitter->is_looping && emitter->duration <= 0.0f)
            {
                transform->is_active = false;
                continue;
            }

            // --- Emission Rate Handling ---
            // Increase the accumulator based on rate_over_time.
            emitter->emissionAccumulator += deltaTime * emitter->particleEmissionRate.rate_over_time;
            int particlesToEmit = static_cast<int>(emitter->emissionAccumulator);
            // Subtract the integer count from the accumulator.
            emitter->emissionAccumulator -= particlesToEmit;

            // Emit the required number of particles.
            for (int i = 0; i < particlesToEmit; ++i)
            {
                SpawnParticles(emitterEntity);
            }
        }

        // --- PARTICLE UPDATE: Process all Particle components (individual particles) ---
        // Collect IDs of particles that need to be deleted.
        std::vector<FlexECS::EntityID> m_entities_to_delete;
        for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<ParticleSystem::Particle>())
        {
            auto particle = elem.GetComponent<ParticleSystem::Particle>();

            // Update lifetime.
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

            // Update particle position based on its velocity.
            elem.GetComponent<Position>()->position +=
                static_cast<Vector3>(elem.GetComponent<Rigidbody>()->velocity) * deltaTime;

            // Calculate normalized lifetime (0 = expired, 1 = just spawned).
            float normalizedTime = 1.0f - (particle->currentLifetime / particle->totalLifetime);

            // Interpolate particle properties based on its lifetime.
            particle->currentSpeed = FlexMath::Lerp(particle->start_speed, particle->end_speed, normalizedTime);
            particle->currentSize = FlexMath::Lerp(particle->start_size, particle->end_size, normalizedTime);
            particle->currentColor = Lerp(particle->start_color, particle->end_color, normalizedTime);

            // Update the corresponding Scale component for rendering.
            elem.GetComponent<Scale>()->scale = Vector3(particle->currentSize, particle->currentSize, particle->currentSize);

            // Optionally: add gravity or drag here.
        }

        // Destroy all expired particles.
        auto scene = FlexECS::Scene::GetActiveScene();
        for (auto entity : m_entities_to_delete)
        {
            scene->DestroyEntity(entity);
            Editor::GetInstance().GetSystem<SelectionSystem>()->DeselectEntity(entity);
        }
    }

    /*!
     * \brief Spawns a particle using a pooled inactive particle if available; otherwise, creates a new one.
     *
     * \param emitterEntity The emitter entity from which to spawn a particle.
     */
    void ParticleSystemLayer::SpawnParticles(FlexECS::Entity& emitterEntity)
    {
        FlexECS::Entity particleEntity;
        bool foundInactive = false;

        // Look for an inactive particle in the pool.
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

    /*!
     * \brief Creates a new particle entity.
     *
     * This function creates a new particle entity with initial properties derived from the emitter.
     *
     * \param emitterEntity The emitter entity from which to derive the particle properties.
     */
    void ParticleSystemLayer::CreateNewParticles(FlexECS::Entity& emitterEntity)
    {
        auto emitter = emitterEntity.GetComponent<ParticleSystem>();

        FlexECS::Entity particleEntity = FlexECS::Scene::GetActiveScene()->CreateEntity();
        particleEntity.AddComponent<Position>({ emitterEntity.GetComponent<Position>()->position });
        particleEntity.AddComponent<Rotation>({ Vector3{0, 0, 0} });
        particleEntity.AddComponent<Scale>({ { emitter->start_size, emitter->start_size, emitter->start_size } });
        particleEntity.AddComponent<Transform>({});
        particleEntity.AddComponent<Sprite>({ emitter->particlesprite_handle, -1 });

        // --- Calculate the initial velocity ---
        Vector3 direction = CalculateInitialVelocity(emitter->particleEmissionShapeIndex);
        if (auto rotationComp = emitterEntity.GetComponent<Rotation>())
        {
            direction = RotateVector(direction, rotationComp->rotation);
        }
        Vector3 velocity = direction * emitter->start_speed;
        particleEntity.AddComponent<Rigidbody>({ velocity, emitter->is_static });

        if (emitter->is_collidable) // Pending proper collision components.
        {
            particleEntity.AddComponent<BoundingBox2D>({});
            particleEntity.AddComponent<AABB>({});
        }

        // --- Initialize the Particle (runtime state) component ---
        ParticleSystem::Particle particle;
        particle.totalLifetime = emitter->lifetime;
        particle.currentLifetime = emitter->lifetime;
        particle.start_speed = emitter->start_speed;
        particle.end_speed = emitter->end_speed;
        particle.start_size = emitter->start_size;
        particle.end_size = emitter->end_size;
        particle.start_color = emitter->start_color;
        particle.end_color = emitter->end_color;
        // Set initial rendering properties matching the starting state.
        particle.currentSpeed = particle.start_speed;
        particle.currentSize = particle.start_size;
        particle.currentColor = particle.start_color;
        // Add the Particle component to the new entity.
        particleEntity.AddComponent<ParticleSystem::Particle>(particle);
    }

    /*!
     * \brief Reinitializes an inactive (pooled) particle entity for reuse.
     *
     * This function resets the properties of an inactive particle entity based on the emitter's
     * properties and reactivates it.
     *
     * \param emitterEntity The emitter entity from which to derive the particle properties.
     * \param particleEntity The inactive particle entity to be reused.
     */
    void ParticleSystemLayer::PoolOldParticles(FlexECS::Entity& emitterEntity, FlexECS::Entity& particleEntity)
    {
        auto emitter = emitterEntity.GetComponent<ParticleSystem>();

        particleEntity.GetComponent<Position>()->position = { emitterEntity.GetComponent<Position>()->position };
        particleEntity.GetComponent<Rotation>()->rotation = { Vector3{0, 0, 0} };
        particleEntity.GetComponent<Scale>()->scale = Vector3{ emitter->start_size, emitter->start_size, emitter->start_size };
        particleEntity.GetComponent<Transform>()->is_active = true;
        particleEntity.GetComponent<Sprite>()->sprite_handle = emitter->particlesprite_handle;

        // --- Calculate the initial velocity ---
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

        // Optionally add collision components if necessary.
        // if (emitter->is_collidable)
        // {
        //     particleEntity.AddComponent<BoundingBox2D>({});
        //     particleEntity.AddComponent<AABB>({});
        // }

        // --- Reinitialize the Particle (runtime state) component ---
        ParticleSystem::Particle particle;
        auto particleComp = particleEntity.GetComponent<ParticleSystem::Particle>();
        particleComp->totalLifetime = emitter->lifetime;
        particleComp->currentLifetime = emitter->lifetime;
        particleComp->start_speed = emitter->start_speed;
        particleComp->end_speed = emitter->end_speed;
        particleComp->start_size = emitter->start_size;
        particleComp->end_size = emitter->end_size;
        particleComp->start_color = emitter->start_color;
        particleComp->end_color = emitter->end_color;
        // Set initial rendering properties matching the starting state.
        particleComp->currentSpeed = particle.start_speed;
        particleComp->currentSize = particle.start_size;
        particleComp->currentColor = particle.start_color;
    }

    /*!
     * \brief Calculates the initial velocity vector for a new particle based on the emission shape.
     *
     * \param shape The particle emission shape index.
     * \return A Vector3 representing the initial velocity direction.
     */
    Vector3 ParticleSystemLayer::CalculateInitialVelocity(const int shape)
    {
        switch (shape)
        {
        case ParticleEmitShape::Sphere:
            // Random direction in any direction.
            return RandomUnitVector();

        case ParticleEmitShape::Hemisphere:
        {
            // Use the sphere generator then restrict to one hemisphere.
            Vector3 v = RandomUnitVector();
            // For example, assume the hemisphere is in the positive Y direction.
            if (v.y < 0.0f)
                v.y = -v.y;
            return v.Normalize();
        }

        case ParticleEmitShape::Cone:
            // For cone shape, assume a fixed cone half-angle (e.g., 30 degrees).
            return RandomUnitVectorCone(30.0f);

        case ParticleEmitShape::Box:
            // For a box, generate a random direction from the cube.
            return RandomUnitVectorCube();

        default:
            // Fallback: default upward direction.
            return Vector3(0.0f, 1.0f, 0.0f);
        }
    }

} // namespace Editor
