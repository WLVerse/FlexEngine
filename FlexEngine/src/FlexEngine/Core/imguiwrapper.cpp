#include "pch.h"

#include "imguiwrapper.h" // <imgui.h> <imgui_internal.h>

//#include <imgui.h>
//#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "application.h"

#define ImColor(R,G,B,A) ImVec4(R / 255.f, G / 255.f, B / 255.f, A / 255.f);

namespace
{
  void CustomImguiStyle(void)
  {
    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameRounding = 0.0f;

    #pragma region Sizes

    // main
    style.WindowPadding = ImVec2(12.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(1.0f, 1.0f);
    style.IndentSpacing = 24.0f;
    style.ScrollbarSize = 16.0f;
    style.GrabMinSize = 16.0f;

    // borders
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    style.TabBarBorderSize = 1.0f;

    // rounding
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 4.0f;

    // tables
    style.CellPadding = ImVec2(12.0f, 5.0f);
    style.TableAngledHeadersAngle = 35.0f;

    // widgets
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
    style.SeparatorTextBorderSize = 3.0f;
    style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
    style.SeparatorTextPadding = ImVec2(24.0f, 6.0f);
    style.LogSliderDeadzone = 4.0f;

    // docking
    //style.DockingSplitterSize = 4.0f;

    // misc
    style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);

    #pragma endregion

    #pragma region Colors

    style.Colors[ImGuiCol_Text]                   = ImColor(255, 255, 255, 255);
    style.Colors[ImGuiCol_TextDisabled]           = ImColor(120, 120, 120, 255);

    style.Colors[ImGuiCol_WindowBg]               = ImColor( 22,  22,  22, 255);
    style.Colors[ImGuiCol_ChildBg]                = ImColor(  0,   0,   0,   0);
    style.Colors[ImGuiCol_PopupBg]                = ImColor( 33,  33,  33, 255);

    style.Colors[ImGuiCol_Border]                 = ImColor(120, 120, 120,  80);
    style.Colors[ImGuiCol_BorderShadow]           = ImColor(  0,   0,   0,   0);

    style.Colors[ImGuiCol_FrameBg]                = ImColor( 50,  50,  50, 160);
    style.Colors[ImGuiCol_FrameBgHovered]         = ImColor( 70,  70,  70, 160);
    style.Colors[ImGuiCol_FrameBgActive]          = ImColor(120, 120, 120, 160);

    style.Colors[ImGuiCol_TitleBg]                = ImColor( 22,  22,  22, 255);
    style.Colors[ImGuiCol_TitleBgActive]          = ImColor( 75,  60, 210, 255);
    style.Colors[ImGuiCol_TitleBgCollapsed]       = ImColor( 22,  22,  22, 255);

    style.Colors[ImGuiCol_MenuBarBg]              = ImColor( 33,  33,  33, 255);

    style.Colors[ImGuiCol_ScrollbarBg]            = ImColor(120, 120, 120,  20);
    style.Colors[ImGuiCol_ScrollbarGrab]          = ImColor(120, 120, 120,  80);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImColor(120, 120, 120, 255);

    style.Colors[ImGuiCol_CheckMark]              = ImColor(135, 135, 255, 255); // ImColor(120, 255, 120, 255);

    style.Colors[ImGuiCol_SliderGrab]             = ImColor(210, 210, 210,  80);
    style.Colors[ImGuiCol_SliderGrabActive]       = ImColor(210, 210, 210, 210);

    style.Colors[ImGuiCol_Button]                 = ImColor(120, 120, 120,  80);
    style.Colors[ImGuiCol_ButtonHovered]          = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_ButtonActive]           = ImColor(120, 120, 120, 255);

    style.Colors[ImGuiCol_Header]                 = ImColor( 50,  50,  50, 160);
    style.Colors[ImGuiCol_HeaderHovered]          = ImColor( 75,  60, 210, 210);
    style.Colors[ImGuiCol_HeaderActive]           = ImColor( 75,  60, 210, 255);

    style.Colors[ImGuiCol_Separator]              = ImColor( 66,  66,  66, 160);
    style.Colors[ImGuiCol_SeparatorHovered]       = ImColor( 66,  66,  66, 210);
    style.Colors[ImGuiCol_SeparatorActive]        = ImColor( 66,  66,  66, 255);

    style.Colors[ImGuiCol_ResizeGrip]             = ImColor(120, 120, 120,  80);
    style.Colors[ImGuiCol_ResizeGripHovered]      = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_ResizeGripActive]       = ImColor(120, 120, 120, 255);

    style.Colors[ImGuiCol_Tab]                    = ImColor( 75,  60, 210, 120);
    style.Colors[ImGuiCol_TabHovered]             = ImColor( 75,  60, 210, 210);
    style.Colors[ImGuiCol_TabActive]              = ImColor( 75,  60, 210, 255);
    style.Colors[ImGuiCol_TabUnfocused]           = ImColor( 75,  60, 210, 120);
    style.Colors[ImGuiCol_TabUnfocusedActive]     = ImColor( 22,  22,  22, 255); // ImColor( 75,  60, 210, 255);

    style.Colors[ImGuiCol_DockingPreview]         = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_DockingEmptyBg]         = ImColor( 50,  50,  50, 255);

    style.Colors[ImGuiCol_PlotLines]              = ImColor(120, 120, 120, 255);
    style.Colors[ImGuiCol_PlotLinesHovered]       = ImColor( 75,  60, 210, 255);
    style.Colors[ImGuiCol_PlotHistogram]          = ImColor(120, 120, 120, 255);
    style.Colors[ImGuiCol_PlotHistogramHovered]   = ImColor( 75,  60, 210, 255);

    style.Colors[ImGuiCol_TableHeaderBg]          = ImColor(120, 120, 120, 210);
    style.Colors[ImGuiCol_TableBorderStrong]      = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_TableBorderLight]       = ImColor(120, 120, 120, 120);
    style.Colors[ImGuiCol_TableRowBg]             = ImColor( 50,  50,  50, 120);
    style.Colors[ImGuiCol_TableRowBgAlt]          = ImColor( 70,  70,  70, 120);

    style.Colors[ImGuiCol_TextSelectedBg]         = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_DragDropTarget]         = ImColor(120, 120, 120, 160);
    style.Colors[ImGuiCol_NavHighlight]           = ImColor(120, 120, 120, 160);

    style.Colors[ImGuiCol_NavWindowingHighlight]  = ImColor(255, 255, 255, 179); // default value
    style.Colors[ImGuiCol_NavWindowingDimBg]      = ImColor(204, 204, 204,  51); // default value
    style.Colors[ImGuiCol_ModalWindowDimBg]       = ImColor(204, 204, 204,  89); // default value

    #pragma endregion

    style.CurveTessellationTol = 1.25f;       // default values
    style.CircleTessellationMaxError = 0.30f; // default values
    style.Alpha = 1.0f;                       // default values
    style.DisabledAlpha = 0.60f;              // default values
  }
}

namespace FlexEngine
{

  ImGuiContext* ImGuiWrapper::Init(Window* window)
  {
    FLX_FLOW_FUNCTION();

    IMGUI_CHECKVERSION();

    ImGuiContext* imgui_context = ImGui::CreateContext();
    // always remember to set the current context before running imgui functions
    ImGui::SetCurrentContext(imgui_context);

    ImGuiIO& io = ImGui::GetIO();

    // set config flags
    // todo: refactor this to window props
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // disable internal mouse cursor handling

    // set style
    //ImGui::StyleColorsDark();
    CustomImguiStyle();

    // load font
    io.Fonts->AddFontFromFileTTF("..\\FlexEngine\\assets\\fonts\\Noto_Sans\\static\\NotoSans-Regular.ttf", 21.f);

    // setup platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init(window->Props().opengl_version_text);

    return imgui_context;
  }

  void ImGuiWrapper::Shutdown(ImGuiContext* imgui_context)
  {
    FLX_FLOW_FUNCTION();

    // always remember to set the current context before running imgui functions
    ImGui::SetCurrentContext(imgui_context);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imgui_context);
  }

  void ImGuiWrapper::BeginFrame()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGuiWrapper::EndFrame()
  {
    GLFWwindow* glfwwindow = Application::GetCurrentWindow()->GetGLFWWindow();
    ImGuiIO& io = ImGui::GetIO();
    int width, height;
    glfwGetWindowSize(glfwwindow, &width, &height);
    io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    // For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
  }

  unsigned int ImGuiWrapper::GetActiveWidgetID()
  {
    return GImGui->ActiveId;
  }

}