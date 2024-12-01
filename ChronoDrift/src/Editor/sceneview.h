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
#include "FlexEngine/Renderer/cameramanager.h"
#include "Components/rendering.h"

namespace ChronoDrift
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
		void HandleMouseAndKeyboardEvents();
		FlexEngine::FlexECS::Entity FindClickedEntity();
		FlexEngine::Vector4 GetWorldClickPosition();
		ImVec2 WorldToScreen(const FlexEngine::Vector2& position);
		void DrawGizmos();

		bool m_gizmo_hovered;
		bool m_dragging;
		Position m_recorded_position;
		Scale		 m_recorded_scale;
		Rotation m_recorded_rotation;

		GizmoType m_current_gizmo_type = GizmoType::TRANSLATE;

		ImVec2 m_viewport_size;
		ImVec2 m_viewport_position;
		ImVec2 m_viewport_screen_position;

		std::unique_ptr<FlexEngine::CameraData> m_EditorCam;
		void UpdateEditorCam();
		#endif
	};
}
