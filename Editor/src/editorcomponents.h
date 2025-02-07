/*!************************************************************************
 * COMPONENT VIEWER LAYER [https://wlverse.web.app]
 * editorcomponents.h - EDITOR
 *
 * This file implements the component viewer definitions and functions for the
 * editor. It is responsible for defining how various components are displayed,
 * manipulated, and registered within the editor environment.
 *
 * Key functionalities include:
 * - Defining component viewer macros for common components such as Position,
 *   Rotation, Scale, Transform, ZIndex, and Parent.
 * - Implementing manual component viewers for components that require custom
 *   add/remove logic (e.g., Audio and Sprite).
 * - Providing detailed viewer functionality for advanced components like Animator,
 *   Camera, Text, ParticleSystem, Physics (BoundingBox2D and Rigidbody),
 *   Scripting, and UI elements.
 * - Registering all component viewers with the editor's component viewer system.
 *
 * The design offers a modular and extensible approach for editing component data,
 * allowing for quick inspection and modification of entity components within the
 * editor.
 *
 * AUTHORS
 * [20%] Chan Wen Loong (wenloong.c@digipen.edu)
 *   - Sub Author
 * [20%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Sub Author
 * ADD HERE
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/
#pragma once
#include "FlexEngine.h"

namespace Editor
{
  void RegisterComponents();
}