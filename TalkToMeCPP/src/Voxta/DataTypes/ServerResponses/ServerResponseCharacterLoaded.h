// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ServerResponseBase.h"
#include <string_view>

namespace Voxta::DataTypes::ServerResponses
{
	struct ServerResponseCharacterLoaded : ServerResponseBase
	{
		ServerResponseType GetType() final
		{
			return ServerResponseType::CHARACTER_LOADED;
		}

		explicit ServerResponseCharacterLoaded(std::string_view characterId, bool enableThinkingSpeech) :
			m_characterId(characterId), m_enableThinkingSpeech(enableThinkingSpeech)
		{
		}

		std::string_view m_characterId;
		bool m_enableThinkingSpeech;
	};
}