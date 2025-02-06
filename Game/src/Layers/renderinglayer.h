/*!************************************************************************
 * RENDERING LAYER [https://wlverse.web.app]
 * renderinglayer.h - GAME
 *
 * This file implements the RenderingLayer class for the editor. The layer is
 * responsible for handling rendering operations, including hotloading a rendering
 * DLL and drawing various scene components such as sprites, text, and animated
 * objects.
 *
 * Key functionalities include:
 * - Enabling and disabling OpenGL blending on layer attach/detach.
 * - Updating the transform components of entities.
 * - Handling animator updates for animated sprites.
 * - Queuing and rendering sprites and text using the OpenGLRenderer.
 *
 * The design provides a modular approach to rendering, allowing for hotloading and
 * quick updates to the rendering DLL.
 *
 * AUTHORS
 * [70%] Chan Wen Loong (wenloong.c@digipen.edu)
 *   - Main Author
 * [30%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Sub Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    /*!
     * \class RenderingLayer
     * \brief Layer responsible for handling rendering operations in the game.
     *
     * The RenderingLayer manages various rendering tasks such as enabling/disabling
     * OpenGL blending, updating transformation matrices, processing animator updates,
     * and queuing the rendering of sprites and text. The design supports hotloading of
     * the rendering DLL for quick iteration and updates.
     */
    class RenderingLayer : public FlexEngine::Layer
    {
    public:
        /*!
         * \brief Constructor for the RenderingLayer.
         *
         * Initializes the layer with the name "Rendering Layer".
         */
        RenderingLayer() : Layer("Rendering Layer") {}

        /*!
         * \brief Default destructor for the RenderingLayer.
         */
        ~RenderingLayer() = default;

        /*!
         * \brief Called when the RenderingLayer is attached to the engine.
         *
         * This function performs initialization tasks such as enabling OpenGL blending
         * to prepare for rendering operations.
         */
        virtual void OnAttach() override;

        /*!
         * \brief Called when the RenderingLayer is detached from the engine.
         *
         * This function performs cleanup tasks such as disabling OpenGL blending to
         * restore the rendering state.
         */
        virtual void OnDetach() override;

        /*!
         * \brief Updates the rendering operations each frame.
         *
         * This function handles the update of transformation components, animator
         * updates, and queues rendering commands for sprites and text via the OpenGLRenderer.
         * It provides a modular approach to rendering within the game.
         */
        virtual void Update() override;
    };

} // namespace Game
