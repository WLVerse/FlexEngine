/*!************************************************************************
 * RENDERING LAYER [https://wlverse.web.app]
 * renderinglayer.h - EDITOR
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

namespace Editor
{
    /*!
     * \class RenderingLayer
     * \brief Layer responsible for rendering operations within the editor.
     *
     * The RenderingLayer handles rendering updates, manages the render queue,
     * and facilitates the drawing of sprites, text, and animated objects. It
     * also supports hotloading of the rendering DLL for dynamic updates.
     */
    class RenderingLayer : public FlexEngine::Layer
    {
    public:
        /*!
         * \brief Constructor for RenderingLayer.
         *
         * Initializes the layer with the name "Rendering Layer".
         */
        RenderingLayer() : Layer("Rendering Layer") {}

        /*!
         * \brief Default destructor for RenderingLayer.
         */
        ~RenderingLayer() = default;

        /*!
         * \brief Called when the layer is attached to the engine.
         *
         * Enables OpenGL blending and performs any necessary initialization for rendering.
         */
        virtual void OnAttach() override;

        /*!
         * \brief Called when the layer is detached from the engine.
         *
         * Disables OpenGL blending and performs any necessary cleanup.
         */
        virtual void OnDetach() override;

        /*!
         * \brief Updates the rendering components each frame.
         *
         * Handles transform updates, animator updates, and manages the render queue
         * for drawing sprites and text.
         */
        virtual void Update() override;

        /*!
         * \brief Retrieves the current render queue.
         *
         * The render queue consists of entities that need to be rendered, paired
         * with their respective render identifiers.
         *
         * \return A vector containing pairs of render identifiers and corresponding entities.
         */
        std::vector<std::pair<std::string, FlexECS::Entity>> GetRenderQueue();
    };

} // namespace Editor
