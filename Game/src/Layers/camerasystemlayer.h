/*!************************************************************************
 * CAMERA SYSTEM LAYER [https://wlverse.web.app]
 * camerasystemlayer.h - GAME
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

namespace Game
{
    /*!
    * \class CameraSystemLayer
    * \brief Layer responsible for updating and validating cameras for CamManager.
    *
    * The CameraSystemLayer handles the per-frame updates and validation of camera
    * entities. It ensures that the cameras are synchronized with the game engine's
    * camera management system, allowing for dynamic and flexible camera behavior.
    */
    class CameraSystemLayer : public FlexEngine::Layer
    {
    public:
        /*!
        * \brief Constructor for the CameraSystemLayer.
        *
        * Initializes the layer with its designated name.
        */
        CameraSystemLayer() : Layer("Camera System Layer") {}

        /*!
        * \brief Default destructor for the CameraSystemLayer.
        */
        ~CameraSystemLayer() = default;

        /*!
        * \brief Called when the layer is attached to the engine.
        *
        * Perform any necessary initialization tasks for the camera system here.
        */
        virtual void OnAttach() override;

        /*!
        * \brief Called when the layer is detached from the engine.
        *
        * Perform any necessary cleanup tasks for the camera system here.
        */
        virtual void OnDetach() override;

        /*!
        * \brief Updates the camera system each frame.
        *
        * This function is responsible for validating and updating camera entities,
        * ensuring that the CamManager receives the latest camera states.
        */
        virtual void Update() override;
    };

} // namespace Game