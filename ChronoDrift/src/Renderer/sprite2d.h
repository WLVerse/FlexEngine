#pragma once
/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// sprite2d.h
//
// Handles 2D sprite rendering within the game engine, focusing on batch rendering
// for efficient processing of large numbers of sprites in a single draw call.
// The rendering system minimizes OpenGL state changes by queueing draw operations
// and applying them only when necessary.
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
#include "camera2d.h"

using namespace FlexEngine;

namespace ChronoDrift
{
    /*!***************************************************************************
    * \brief
    * Updates the transformation matrices of all active entities in the scene, ensuring correct hierarchical relationships.
    *
    * \param CamManager Pointer to the CameraManager responsible for managing camera data.
    *****************************************************************************/
    void UpdateAllEntitiesMatrix(CameraManager* CamManager);

    /*!***************************************************************************
    * \brief
    * Updates and animates all active entities in the scene that have an animation component.
    *
    * \param GameTimeSpeedModifier A speed multiplier affecting animation progression.
    *****************************************************************************/
    void UpdateAnimationInScene(double GameTimeSpeedModifier);

    /*!***************************************************************************
    * \brief
    * Renders all the 2D sprites in the scene. This function handles the necessary
    * steps for rendering, including setting up shader properties, handling
    * post-processing, and batch rendering for efficiency.
    *****************************************************************************/
	void RenderSprite2D();
}
