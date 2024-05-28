// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace Voxta::DataTypes::ServerResponses
{
	struct CharacterLoadedVoiceData
	{
		explicit CharacterLoadedVoiceData(const std::map<std::string, std::string>& voiceParameters, std::string_view voiceLabel)
			: m_voiceParameters(voiceParameters), m_voiceLabel(voiceLabel)
		{
		}

		const std::map<std::string, std::string> m_voiceParameters;
		const std::string m_voiceLabel;
	};

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
			std::vector<CharacterLoadedVoiceData> voiceOverrideConfigs) :
			m_characterId(characterId), m_enableThinkingSpeech(enableThinkingSpeech),
			m_voiceData(voiceOverrideConfigs)
		{
		}

		const std::string m_characterId;
		const bool m_enableThinkingSpeech;
		const std::vector<CharacterLoadedVoiceData> m_voiceData;
	};
}