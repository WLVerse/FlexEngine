// WLVERSE [https://wlverse.web.app]
// FlexEngine.h
// 
// FlexEngine Main Include File
// This file includes all the necessary files to use the FlexEngine library.
// 
// Sections:
// - Core  . . . . . . The core of the engine. Required for all applications.
// - Tools . . . . . . Tools for logging, timing, and other utilities.
// - Wrappers  . . . . Wrappers for external libraries and tools.
// - Data Structures . Data structures used in the engine.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once


/* |----------------------------| */
/* |----------- Core -----------| */
/* |----------------------------| */

// Contains the application class.
// Inherit from FlexEngine::Application in the entry point to create a new application.
#include "FlexEngine/Core/application.h"

// Contains the layer class.
// Inherit from FlexEngine::Layer to create a new layer.
// Layers are used to separate different parts of the application.
// Layers are updated and rendered in the order they are pushed.
#include "FlexEngine/Core/layer.h"

// Manages all the assets in the application.
// Load and store assets like textures and shaders.
// All assets should be referenced by their key, which will
// then be used to retrieve the asset.
#include "FlexEngine/AssetManager/assetmanager.h"


/* |-----------------------------| */
/* |----------- Tools -----------| */
/* |-----------------------------| */

// Logger for logging messages to a file
// 
// Use Log::Debug for debugging information. (only logged in debug mode)
// Use Log::Info for general information.
// Use Log::Warning for warnings.
// Use Log::Error for errors.
// Use Log::Fatal for fatal errors. (this will terminate the application)
// 
// Flow logging is used to log the flow of the application and track
// the flow of systems within the application.
// 
// Use FLX_FLOW_FUNCTION() to log the current function in the flow.
// Use FLX_FLOW_BEGINSCOPE() to log the start of a system scope flow.
// Use FLX_FLOW_ENDSCOPE() to log the end of a system scope flow.
#include "FlexEngine/flexlogger.h"
#include "FlexEngine/flexprofiler.h"

// Assertion wrapper for handling fatal errors.
// These are used to check for destructive errors like memory leaks.
// The application will be terminated if an assertion fails.
// This hooks to the logger to log the error before terminating.
// 
// Use FLX_INTERNAL_ASSERT for assertions before the application is functional.
// Use FLX_CORE_ASSERT for assertions for the application core.
// Use FLX_ASSERT in place of regular assertions.
// Use FLX_NULLPTR_ASSERT for checking pointers.
#include "FlexEngine/Wrapper/flexassert.h"

// Formatter for storing metadata in files.
// Uses a simple key-value pair system to store metadata in files.
#include "FlexEngine/flexformatter.h"

// UUID class for generating unique identifiers.
#include "FlexEngine/uuid.h"

// FlexID class for generating special unique identifiers.
// The first 32 bits are the ID, the next 28 bits are the generation, and the last 4 bits are the flags.
#include "FlexEngine/flexid.h"

// Scoped timer to measure time taken by functions.
// Macros are provided to measure the time taken by a function.
#include "FlexEngine/timer.h"

// Math functions and constants.
// Contains constants like PI and EPSILON.
// Contains conversion functions for degrees and radians.
// Contains math functions for common operations like clamp.
// Implements Vector2, Vector3, Vector4.
// Implements Matrix4x4 for transformations.
// Implements Quaternion for rotations.
#include "FlexEngine/FlexMath/mathconstants.h"
#include "FlexEngine/FlexMath/mathconversions.h"
#include "FlexEngine/FlexMath/mathfunctions.h"
#include "FlexEngine/FlexMath/vector1.h"
#include "FlexEngine/FlexMath/vector2.h"
#include "FlexEngine/FlexMath/vector3.h"
#include "FlexEngine/FlexMath/vector4.h"
#include "FlexEngine/FlexMath/matrix1x1.h"
#include "FlexEngine/FlexMath/matrix4x4.h"
#include "FlexEngine/FlexMath/quaternion.h"

// Used to manage states.
// Make your own states by inheriting from FlexEngine::State.
// Macros are provided to make the state management easier.
#include "FlexEngine/StateManager/statemanager.h"

// Input handling abstracted from GLFW.
// Use GLFW key codes.
// Currently does not support gamepads.
#include "FlexEngine/input.h"


/* |-----------------------------| */
/* |---------- Wrappers ---------| */
/* |-----------------------------| */
#ifndef GAME
// Wraps the ImGui library for easier use.
// Includes the ImGui library.
#include "FlexEngine/Core/imguiwrapper.h"
#endif 
// Contains the path, file, and filelist class.
// Has the Extension namespace with a list of safe and supported extensions.
// Handles the functionality to read and write files.
// The path class is used to ensure the validity of the path. Can throw exceptions.
#include "FlexEngine/Wrapper/path.h"
#include "FlexEngine/Wrapper/file.h"
#include "FlexEngine/Wrapper/filelist.h"

/* |-----------------------------| */
/* |---------- Renderer ---------| */
/* |-----------------------------| */

// Contains the renderer class.
// The current implementation is exclusively for OpenGL.
#include "FlexEngine/Renderer/OpenGL/openglrenderer.h"
// The current implementation is exclusively for OpenGL. For 2D sprites
#include "FlexEngine/Renderer/OpenGL/openglspriterenderer.h"
// The current implementation is exclusively for OpenGL. For freetype text
#include "FlexEngine/Renderer/OpenGL/opengltextrenderer.h"

#include "FlexEngine/Renderer/OpenGL/opengldebugrenderer.h"

// Stores one vertex for the mesh.
// The current implementation is exclusively for OpenGL.
#include "FlexEngine/Renderer/OpenGL/openglvertex.h"

// Handles the creation and destruction of vertex and index buffers.
// This is platform-independent and can be used with any graphics API.
// Although the current implementation is exclusively for OpenGL.
#include "FlexEngine/Renderer/buffer.h"
#include "FlexEngine/Renderer/OpenGL/openglbuffer.h"

// Load textures from files.
// The current implementation is exclusively for OpenGL.
#include "FlexEngine/Renderer/OpenGL/opengltexture.h"

#include "FlexEngine/Renderer/OpenGL/openglmaterial.h"

// Contains the FlexEngine mesh class.
// This class is used to store vertices, indices, and textures.
// The current implementation is exclusively for OpenGL.
#include "FlexEngine/Renderer/OpenGL/openglmesh.h"

#include "FlexEngine/Renderer/OpenGL/openglmodel.h"

// Load and compile shaders from files.
// The current implementation is exclusively for OpenGL.
#include "FlexEngine/Renderer/OpenGL/openglshader.h"


/* |-----------------------------| */
/* |------ Data Structures ------| */
/* |-----------------------------| */

// Contains the FlexECS system.
// This uses the Archetype-based Entity-Component-System architecture.
#include "FlexEngine/FlexECS/datastructures.h"

// Two way queue for storing and executing functions.
#include "FlexEngine/DataStructures/functionqueue.h"

// Specialized function queue for storing and executing functions for freeing
// heap objects after execution.
#include "FlexEngine/DataStructures/freequeue.h"

// Data structure for a range of values.
// Use Get() to generate random values within the range.
// The number that is generated is inclusive of the min and max values.
// Can also be used to store a range of values by getting min and max.
#include "FlexEngine/DataStructures/range.h"

/* |-----------------------------| */
/* |------        FMOD     ------| */
/* |-----------------------------| */
#include "FlexEngine/FMOD/FMODWrapper.h"
