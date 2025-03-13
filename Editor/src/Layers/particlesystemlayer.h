/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// particlesystemlayer.h
//
// Declares the ParticleSystemLayer class, which is responsible for managing
// particle system simulation and rendering in the Editor module. This layer
// handles the spawning of new particles, pooling of old particles, and updating
// active particle entities.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{
    /**
     * @brief Manages particle system simulation and rendering.
     *
     * The ParticleSystemLayer class handles particle spawning, updating,
     * and pooling for efficient simulation within the editor environment.
     */
    class ParticleSystemLayer : public FlexEngine::Layer
    {
        // Container for active particle entities.
        std::vector<FlexECS::Entity> m_ParticleEntities;

        /**
         * @brief Creates new particles for a given emitter entity.
         *
         * @param emitterEntity The entity acting as the particle emitter.
         */
        void CreateNewParticles(FlexECS::Entity& emitterEntity);

        /**
         * @brief Pools (recycles) an old particle entity.
         *
         * @param emitterEntity The emitter entity that spawned the particle.
         * @param particleEntity The particle entity to be pooled.
         */
        void PoolOldParticles(FlexECS::Entity& emitterEntity, FlexECS::Entity& particleEntity);

    public:
        // Constructs the ParticleSystemLayer with the default layer name "Particle System Layer".
        ParticleSystemLayer() : Layer("Particle System Layer") {}

        // Default destructor.
        ~ParticleSystemLayer() = default;

        /**
         * @brief Called when the layer is attached to the application.
         *
         * Use this function to initialize particle system resources.
         */
        virtual void OnAttach() override;

        /**
         * @brief Called when the layer is detached from the application.
         *
         * Use this function to clean up particle system resources.
         */
        virtual void OnDetach() override;

        /**
         * @brief Main update function called every frame.
         *
         * Handles updating particle simulation and rendering.
         */
        virtual void Update() override;

        /**
         * @brief Spawns new particles using the specified emitter entity.
         *
         * @param emitterEntity The entity from which particles are spawned.
         */
        void SpawnParticles(FlexECS::Entity& emitterEntity);

        /**
         * @brief Calculates the initial velocity vector for a particle.
         *
         * @param shape An integer representing the emitter shape or configuration.
         * @return A Vector3 containing the calculated initial velocity.
         */
        Vector3 CalculateInitialVelocity(const int shape);
    };

}
