/*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      imguipayloads.h
\author    [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
\date      27/11/2024
\brief     IMGUI Payload declarations


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once


namespace ChronoDrift
{
	enum PayloadTags
	{
		UNKNOWN,
		IMAGE,
		PREFAB,
		ENTITY,
		SHADER,
		SCENE,
		AUDIO,
		FONT,

		PAYLOADTAGS_LAST
	};

	inline const char* GetPayloadTagString(PayloadTags tag)
	{
		switch (tag)
		{
		case ChronoDrift::IMAGE: return "IMAGE";
		case ChronoDrift::PREFAB: return "PREFAB";
		case ChronoDrift::ENTITY: return "ENTITY";
		case ChronoDrift::SHADER: return "SHADER";
		case ChronoDrift::SCENE: return "SCENE";
		case ChronoDrift::AUDIO: return "AUDIO";
		case ChronoDrift::FONT: return "FONT";
		default: return "UNKNOWN";
		}
	}

}