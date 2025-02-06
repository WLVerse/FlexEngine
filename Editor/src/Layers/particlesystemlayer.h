/*!************************************************************************
 * PARTICLE SYSTEM LAYER [https://wlverse.web.app]
 * particlesystemlayer.h - EDITOR
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
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{
    /*!
     * \class ParticleSystemLayer
     * \brief Layer for managing particle emitters and particle updates.
     *
     * The ParticleSystemLayer class is responsible for updating emitter components,
     * handling the spawning of particles (either by reusing inactive ones from a pool or
     * by creating new ones), and updating individual particle properties over time.
     * This ensures efficient management of particle effects in the scene.
     */
    class ParticleSystemLayer : public FlexEngine::Layer
    {
        /// Container for active particle entities.
        std::vector<FlexECS::Entity> m_ParticleEntities;

        /*!
         * \brief Creates a new particle entity based on the emitter's properties.
         *
         * \param emitterEntity The emitter entity from which to derive particle properties.
         */
        void CreateNewParticles(FlexECS::Entity& emitterEntity);

        /*!
         * \brief Reinitializes an inactive (pooled) particle entity for reuse.
         *
         * \param emitterEntity The emitter entity from which to derive particle properties.
         * \param particleEntity The inactive particle entity to be reused.
         */
        void PoolOldParticles(FlexECS::Entity& emitterEntity, FlexECS::Entity& particleEntity);

    public:
        /*!
         * \brief Constructor for ParticleSystemLayer.
         *
         * Initializes the layer with the name "Particle System Layer".
         */
        ParticleSystemLayer() : Layer("Particle System Layer") {}

        /*!
         * \brief Default destructor for ParticleSystemLayer.
         */
        ~ParticleSystemLayer() = default;

        /*!
         * \brief Called when the layer is attached to the engine.
         *
         * Performs initialization tasks such as loading necessary resources.
         */
        virtual void OnAttach() override;

        /*!
         * \brief Called when the layer is detached from the engine.
         *
         * Performs cleanup tasks such as clearing the container of particle entities.
         */
        virtual void OnDetach() override;

        /*!
         * \brief Updates emitter and particle components each frame.
         *
         * Handles the update of emitter timers, emission of particles based on rate,
         * updating of individual particle properties (lifetime, position, etc.), and
         * cleanup of expired particles.
         */
        virtual void Update() override;

        /*!
         * \brief Spawns a particle from the given emitter.
         *
         * Attempts to reuse an inactive particle entity from the pool; if none are available,
         * a new particle entity is created.
         *
         * \param emitterEntity The emitter entity from which to spawn a particle.
         */
        void SpawnParticles(FlexECS::Entity& emitterEntity);

        /*!
         * \brief Calculates the initial velocity vector for a particle based on the emission shape.
         *
         * \param shape The particle emission shape index.
         * \return A Vector3 representing the initial velocity direction.
         */
        Vector3 CalculateInitialVelocity(const int shape);
    };

} // namespace Editor
