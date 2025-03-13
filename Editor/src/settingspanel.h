/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// settingspanel.h
//
// Settings Panel for the editor, using FlexPrefs.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once

#include "editorpanel.h"
#include "FlexEngine.h"

namespace Editor {

	class SettingsPanel : public EditorPanel {
	public:
		SettingsPanel();
		virtual ~SettingsPanel() {}

		// Called when the panel is first created
		virtual void Init() override;
		// Called once per frame to render the panel
		virtual void Update() override;
		// Called to render out the settings value
		virtual void EditorUI() override;
		// Called when the panel is shutting down
		virtual void Shutdown() override;

		void SetOpen(bool val);
		bool IsOpen();
	private:
		bool m_open = false;  // Controls visibility of the settings window

		// --- Editor Settings ---
		int   m_editorThemeIndex;  // 0: Dark, 1: Light
		float m_editorCameraSpeed;
		bool  m_editorBatching;

		// --- Game Settings ---
		bool  m_gameFullscreen;
		bool  m_gameVSync;
		bool  m_gameBatching;
		int   m_gameResolutionIndex; // e.g., 0: "1920x1080", 1: "1600x900", etc.
		float m_gameVolume;
	};

} // namespace Editor
