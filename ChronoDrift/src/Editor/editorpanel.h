/*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      editorpanel.h
\author    [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
\date      27/11/2024
\brief     Base declaration of base class Editor Panels


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <FlexEngine.h>


class EditorPanel
{

public:
	/************************** Virtual functions to implement *************************/
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void EditorUI() = 0;
	virtual void Shutdown() = 0;

	FlexEngine::Vector2 mouse_to_world;
};