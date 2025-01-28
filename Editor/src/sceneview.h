/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// sceneview.h
//
// Scene Panel for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "editorpanel.h"

namespace Editor
{
	class SceneView : public EditorPanel
	{
		#ifndef GAME
	public:
		void Init();
		void Update();
		void EditorUI();
		void Shutdown();

	private:
		enum GizmoType
		{
			TRANSLATE,
			SCALE,
			ROTATE
		};

		void CalculateViewportPosition();	//position of viewport image and size
		//void HandleMouseAndKeyboardEvents();
		//FlexEngine::FlexECS::Entity FindClickedEntity();
		//FlexEngine::Vector4 GetWorldClickPosition();
		//ImVec2 WorldToScreen(const FlexEngine::Vector2& position);
		//void DrawGizmos();
		//void MoveEditorCam();

		bool m_gizmo_hovered;
		bool m_dragging;

		bool m_dragging_camera{ false };

		GizmoType m_current_gizmo_type = GizmoType::TRANSLATE;

		ImVec2 m_viewport_size;
		ImVec2 m_viewport_position;
		ImVec2 m_viewport_screen_position;

		//std::unique_ptr<FlexEngine::CameraData> m_EditorCam;
		#endif
	};
}
