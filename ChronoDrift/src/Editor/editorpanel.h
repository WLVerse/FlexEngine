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