#pragma once
/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// sprite2d.h
//
// This file is responsible for handling 2D sprite rendering within the game.
// It focuses on batch rendering, enabling the efficient processing of large
// numbers of sprites in a single draw call. The system minimizes OpenGL state
// changes by organizing draw operations into a queue that is flushed once
// all sprites have been prepared.
//
// Key functionalities include:
// - Hierarchical transformation system, where child entities inherit
//   transformations from their parents, enabling complex object groupings
//   and movement.
// - Efficient batch rendering pipeline to reduce draw calls, leveraging
//   modern OpenGL techniques such as VAOs, VBOs, and shader management.
// - Managing the necessary OpenGL resources (shaders, framebuffers, textures)
//   and ensuring optimized memory usage during the rendering process.
// - Ensuring precise alignment of sprites via local transformations and
//   alignment options, with smooth integration into the parent-child matrix
//   relationship.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Developed the hierarchical transformation and batch rendering system
//     to optimize large-scale 2D rendering workflows, including post-processing
//     and shader integration.
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoShift
{
    /*!***************************************************************************
    * \brief
    * Updates the transformation matrix of 2D sprites within the scene. It ensures
    * proper alignment and processing of entities in the scene, particularly
    * their position and orientation in the hierarchy.
    *****************************************************************************/
	void UpdateSprite2DMatrix();

    /*!***************************************************************************
    * \brief
    * Renders all the 2D sprites in the scene. This function handles the necessary
    * steps for rendering, including setting up shader properties, handling
    * post-processing, and batch rendering for efficiency.
    *****************************************************************************/
	void RendererSprite2D();

}
