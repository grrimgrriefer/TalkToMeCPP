// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <string_view>

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for the CharacterLoaded response from the server
	/// Should be pretty obvious.
	/// </summary>
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

		const std::string m_characterId;
		const bool m_enableThinkingSpeech;
	};
}