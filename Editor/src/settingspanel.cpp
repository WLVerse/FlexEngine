#include "settingspanel.h"
#include "editor.h"

namespace Editor {

    SettingsPanel::SettingsPanel()
    {
    }

    void SettingsPanel::Init() 
    {
        // Any one-time initialization for the settings panel can go here.
        FlexPrefs::Load();

        // Load Editor settings
        m_editorCameraSpeed = FlexPrefs::GetFloat("editor.cameraSpeed", 1.0f);
        m_editorThemeIndex = FlexPrefs::GetInt("editor.themeIndex", 0);
        m_editorBatching = FlexPrefs::GetBool("editor.batching", true);

        // Load Game settings
        m_gameFullscreen = FlexPrefs::GetBool("game.fullscreen", false);
        m_gameVSync = FlexPrefs::GetBool("game.vsync", true);
        m_gameBatching = FlexPrefs::GetBool("game.batching", true);
        m_gameResolutionIndex = FlexPrefs::GetInt("game.resolutionIndex", 0);
        m_gameVolume = FlexPrefs::GetFloat("game.volume", 0.75f);
    }

    void SettingsPanel::Update() 
    {
    }

    void SettingsPanel::EditorUI()
    {
        if (!m_open)
            return;

        // Begin a new ImGui window for Settings.
        ImGui::Begin("Settings", &m_open);

        // ----- Editor Settings -----
        if (ImGui::CollapsingHeader("Editor Settings", ImGuiTreeNodeFlags_DefaultOpen)) 
        {
            if (ImGui::SliderFloat("Camera Speed", &m_editorCameraSpeed, 0.1f, 10.0f)) 
            {
                FlexPrefs::SetFloat("editor.cameraSpeed", m_editorCameraSpeed);
            }
            // Use a separate text label and hidden combo label to place the text on the left.
            ImGui::Text("Editor Theme");
            ImGui::SameLine();
            const char* editorThemes[] = { "Dark", "Light" };
            if (ImGui::Combo("##EditorTheme", &m_editorThemeIndex, editorThemes, IM_ARRAYSIZE(editorThemes))) 
            {
                FlexPrefs::SetInt("editor.themeIndex", m_editorThemeIndex);
                // Optionally, apply immediate theme changes here.
                // e.g., (m_editorThemeIndex == 0 ? SetupDarkTheme() : SetupLightTheme());
            }
            if (ImGui::Checkbox("Editor Batching", &m_editorBatching)) 
            {
                FlexPrefs::SetBool("editor.batching", m_editorBatching);
            }
        }
        ImGui::NewLine();

        // ----- Game Settings -----
        if (ImGui::CollapsingHeader("Game Settings", ImGuiTreeNodeFlags_DefaultOpen)) 
        {
            if (ImGui::Checkbox("Fullscreen", &m_gameFullscreen)) 
            {
                FlexPrefs::SetBool("game.fullscreen", m_gameFullscreen);
            }
            if (ImGui::Checkbox("OpenGL VSync", &m_gameVSync)) 
            {
                FlexPrefs::SetBool("game.vsync", m_gameVSync);
            }
            if (ImGui::Checkbox("Game Batching", &m_gameBatching)) 
            {
                FlexPrefs::SetBool("game.batching", m_gameBatching);
            }
            // Dropdown for selecting resolution.
            const char* resolutions[] = { "1920x1080", "1600x900", "1366x768", "1280x720" };
            if (ImGui::Combo("Resolution", &m_gameResolutionIndex, resolutions, IM_ARRAYSIZE(resolutions))) 
            {
                FlexPrefs::SetInt("game.resolutionIndex", m_gameResolutionIndex);
            }
            if (ImGui::SliderFloat("Audio Volume", &m_gameVolume, 0.0f, 1.0f)) 
            {
                FlexPrefs::SetFloat("game.volume", m_gameVolume);
            }
        }

        // ----- Save Button -----
        ImGui::Separator();
        if (ImGui::Button("Save Settings", ImVec2(180, 30)))
        {
            FlexPrefs::Save();
        }
        ImGui::End();
    }

    void SettingsPanel::Shutdown() {
        // Save settings to disk when shutting down.
        FlexPrefs::Save();
    }

    void SettingsPanel::SetOpen(bool val)
    {
        m_open = val;
    }
    bool SettingsPanel::IsOpen()
    {
        return m_open;
    }

} // namespace Editor
