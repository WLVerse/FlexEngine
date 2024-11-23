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
		default: return "UNKNOWN";
		}
	}

}