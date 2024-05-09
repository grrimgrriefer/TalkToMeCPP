// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaResponseBase.h"
#include <string_view>

namespace Voxta::DataTypes
{
	struct VoxtaResponseCharacterLoaded : DataTypes::VoxtaResponseBase
	{
		DataTypes::VoxtaResponseType GetType() final
		{
			return DataTypes::VoxtaResponseType::CHARACTER_LOADED;
		}

		explicit VoxtaResponseCharacterLoaded(std::string_view characterId, bool enableThinkingSpeech) :
			m_characterId(characterId), m_enableThinkingSpeech(enableThinkingSpeech)
		{
		}

		std::string_view m_characterId;
		bool m_enableThinkingSpeech;
	};
}