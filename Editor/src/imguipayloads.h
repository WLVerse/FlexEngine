/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// imguipayloads.h
//
// Interface for simplifying the use of ImGui payloads.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/
#pragma once

namespace Editor
{
	enum PayloadTags
	{
		UNKNOWN,
		IMAGE,
		SPRITESHEET,
		PREFAB,
		ENTITY,
		SHADER,
		SCENE,
		AUDIO,
		FONT,
		VIDEO,

		PAYLOADTAGS_LAST
	};

	inline const char* GetPayloadTagString(PayloadTags tag)
	{
		switch (tag)
		{
		case PayloadTags::IMAGE: return "IMAGE";
		case PayloadTags::SPRITESHEET: return "SPRITESHEET";
		case PayloadTags::PREFAB: return "PREFAB";
		case PayloadTags::ENTITY: return "ENTITY";
		case PayloadTags::SHADER: return "SHADER";
		case PayloadTags::SCENE: return "SCENE";
		case PayloadTags::AUDIO: return "AUDIO";
		case PayloadTags::FONT: return "FONT";
		case PayloadTags::VIDEO: return "VIDEO";
		default: return "UNKNOWN";
		}
	}

}