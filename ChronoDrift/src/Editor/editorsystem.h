#pragma once
#include <FlexEngine.h>


class EditorSystem
{
public:
	/************************** Virtual functions to implement *************************/
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Shutdown() = 0;
};
