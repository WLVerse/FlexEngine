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

/******************************
		SCENE KEYBOARD CONTROLS
*Only works when Scene is in focus

Camera controls:
>	 Middle mouse button / LAlt + Rightclick - Pan camera
>	 Scroll zoomy zoom

Entity Manipulation:
>	 W E R - Gizmos, Translate Scale Rotate respectively
>	 LCtrl + click on entity - multiselect
>	 Delete key - delete

Debug / visualizers:
>	 Shift + T - toggle camera bounds visualizer
*******************************/


namespace Editor
{
	class SceneView : public EditorPanel
	{
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
		void DraggingEntitySelect();
		void FindEntitiesInSelection();

		FlexEngine::Vector4 GetWorldClickPosition();
		FlexEngine::Vector4 ScreenToWorld(ImVec2);
		ImVec2 WorldToScreen(const FlexEngine::Vector2& position);
		void DrawGizmos();
		void MoveEditorCam();
		void DisplayMainCamBounds();

		bool m_gizmo_hovered;
		bool m_dragging_gizmo;

		ImVec2 m_drag_selection_start_point;
		bool m_dragging_selection { false };

		bool m_dragging_camera{ false };
		bool m_display_cam_bounds { true };

		GizmoType m_current_gizmo_type = GizmoType::TRANSLATE;

		ImVec2 m_viewport_size;
		ImVec2 m_viewport_position;
		ImVec2 m_viewport_screen_position;

		FlexEngine::Position m_recorded_position;
		FlexEngine::Scale		 m_recorded_scale;
		FlexEngine::Rotation m_recorded_rotation;

		FlexEngine::FlexECS::Entity m_editor_camera;
		FlexEngine::Vector3 m_camera_position{ 0,0,0 };

	};
}
