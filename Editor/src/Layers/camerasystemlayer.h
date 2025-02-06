/*!************************************************************************
 * CAMERA SYSTEM LAYER [https://wlverse.web.app]
 * camerasystemlayer.h - EDITOR
 *
 * This file defines the CameraSystemLayer class, which is responsible for
 * updating and validating camera entities in the CamManager. The layer ensures
 * that camera states are properly updated each frame, thereby maintaining
 * synchronization and consistency within the game engine's camera management system.
 *
 * Key functionalities include:
 * - Updating camera entities on each frame.
 * - Validating camera states and relaying updates to the CamManager.
 *
 * The design emphasizes modularity and performance, facilitating seamless
 * integration with the engine's layer system and flexible camera management.
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
     * \class CameraSystemLayer
     * \brief Layer responsible for updating and validating camera entities.
     *
     * The CameraSystemLayer class manages the camera entities by updating their
     * states and ensuring they are properly registered with the CamManager. This
     * layer guarantees that the camera management system remains synchronized with
     * the active scene each frame.
     */
    class CameraSystemLayer : public FlexEngine::Layer
    {
    public:
        /*!
         * \brief Constructor for the CameraSystemLayer.
         *
         * Initializes the layer with the name "Camera System Layer".
         */
        CameraSystemLayer() : Layer("Camera System Layer") {}

        /*!
         * \brief Default destructor for the CameraSystemLayer.
         */
        ~CameraSystemLayer() = default;

        /*!
         * \brief Called when the layer is attached to the engine.
         *
         * Performs initialization tasks such as registering active camera entities
         * with the CamManager.
         */
        virtual void OnAttach() override;

        /*!
         * \brief Called when the layer is detached from the engine.
         *
         * Performs cleanup tasks, including unregistering camera entities from the
         * CamManager.
         */
        virtual void OnDetach() override;

        /*!
         * \brief Updates camera entities each frame.
         *
         * Validates and updates camera states by ensuring that each active camera is
         * properly registered, updating its transformation, and relaying necessary
         * updates to the CamManager.
         */
        virtual void Update() override;
    };

} // namespace Editor
