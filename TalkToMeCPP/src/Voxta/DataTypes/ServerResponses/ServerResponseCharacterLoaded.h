// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include "../CharVoiceConfig.h"
#include <string_view>
#include <vector>

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

		explicit ServerResponseCharacterLoaded(std::string_view characterId, bool enableThinkingSpeech,
			std::vector<CharVoiceConfig> voiceOverrideConfigs) :
			m_characterId(characterId), m_enableThinkingSpeech(enableThinkingSpeech),
			m_voiceOverrideConfigs(voiceOverrideConfigs)
		{
		}

		const std::string m_characterId;
		const bool m_enableThinkingSpeech;
		const std::vector<CharVoiceConfig> m_voiceOverrideConfigs;
	};
}