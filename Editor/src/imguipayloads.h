#pragma once


namespace Editor
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
		case PayloadTags::IMAGE: return "IMAGE";
		case PayloadTags::PREFAB: return "PREFAB";
		case PayloadTags::ENTITY: return "ENTITY";
		case PayloadTags::SHADER: return "SHADER";
		case PayloadTags::SCENE: return "SCENE";
		case PayloadTags::AUDIO: return "AUDIO";
		case PayloadTags::FONT: return "FONT";
		default: return "UNKNOWN";
		}
	}

}