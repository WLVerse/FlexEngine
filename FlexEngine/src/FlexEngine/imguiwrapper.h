// WLVERSE [https://wlverse.web.app]
// imguiwrapper.h
// 
// This class is a wrapper to handle the loading and unloading
// of the ImGui context, backend, and store the context pointer.
// One context and backend setup is needed per window.
// 
// Since ImGui is not recommended for DLLs, this class can only work
// by setting the context pointer before using ImGui functions.
// Use the FLX_IMGUI_ALIGNCONTEXT macro to set the context pointer.
// 
// The order of operations is as follows:
// The indentation represents the passing of control over to the
// other DLLs.
// - Init
// - SetContext
// - BeginFrame
//   - SetContext
//   - ImGui Functions
// - EndFrame
// - Shutdown
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#ifdef IMGUI_DISABLE
#error "ImGui is disabled because IMGUI_DISABLE is defined. Please enable ImGui to use this header. If it's supposed to be disabled, look for where you might have accidentally included this header without putting #ifndef IMGUI_DISABLE"
#endif

#include "flx_api.h"

#include "Layer/ilayer.h" // <string>

#include <imgui.h>
#include <imgui_internal.h>

#include "window.h"

namespace FlexEngine
{

  class __FLX_API ImGuiWrapper
  {

    #pragma region Managed RAII Pattern

    // Only loaded once because the first window is the master window.
    // This is because we're using shared contexts.
    // This also helps localize the context pointer to the window.

  private:
    static ImGuiContext* m_imgui_context;

  public:
    static void Init();
    static void Shutdown();

    static ImGuiContext* GetContext();

    // Aligns the context pointer.
    // 
    // ImGui is not recommended to be used in DLLs because of the global context pointer.
    // We must call this function before using ImGui across any DLL boundaries.
    // 
    // !!IMPORTANT!! This cannot be a function. It MUST be a macro because we want
    // ImGui::SetCurrentContext to actually be called wherever this macro is used.
    // If it's a function, it will only set the context for the function's boundary,
    // which is always in this DLL.
    #define FLX_IMGUI_ALIGNCONTEXT() \
      FlexEngine::ImGuiWrapper::Internal_SetCurrentContext(FlexEngine::ImGuiWrapper::GetContext()); \
      ImGui::SetCurrentContext(FlexEngine::ImGuiWrapper::GetContext()); \
      ImGui::SetCurrentViewport(nullptr, (ImGuiViewportP*)ImGui::FindViewportByPlatformHandle(FlexEngine::Application::GetCurrentWindow()->GetGLFWWindow()))
      //ImGui::GetMainViewport()->PlatformHandle = FlexEngine::Application::GetCurrentWindow()->GetGLFWWindow()

    // INTERNAL FUNCTION
    // Passthrough function for ImGui::SetCurrentContext, which
    // is needed if we want to call this function across DLL boundaries.
    // This also sets the main viewport platform handle.
    static void Internal_SetCurrentContext(ImGuiContext* context);

    #pragma endregion

    #pragma region ImGui Extended Functions

    // These functions are wrappers for the ImGui functions
    // by the same name.

  public:
    // Runs the three ImGui new frame functions.
    // 
    // This is called by the window update function and does not
    // need to be called manually anywhere else.
    static void BeginFrame();

    // Runs the ImGui render function and passes the frame to the backend.
    // This also automatically handles any multi-window rendering.
    // 
    // This is called by the window update function and does not
    // need to be called manually anywhere else.
    static void EndFrame();

    #pragma endregion

  };

}