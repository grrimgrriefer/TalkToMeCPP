// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <map>
#include <string>
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
		/// <summary>
		/// Struct that maps the voice data specific to this character.
		/// Internal struct as it is only relevant to the serverReponse,
		/// raw data is stored in the CharData after parsing the response.
		/// </summary>
		struct CharacterLoadedVoiceData
		{
			const std::map<std::string, std::string> m_voiceParameters;
			const std::string m_voiceLabel;

			explicit CharacterLoadedVoiceData(const std::map<std::string, std::string>& voiceParameters,
					std::string_view voiceLabel) :
				m_voiceParameters(voiceParameters),
				m_voiceLabel(voiceLabel)
			{
			}
		};

		const std::string m_characterId;
		const bool m_enableThinkingSpeech;
		const std::vector<CharacterLoadedVoiceData> m_voiceData;

		ServerResponseType GetType() final
		{
			return ServerResponseType::CHARACTER_LOADED;
		}

		explicit ServerResponseCharacterLoaded(std::string_view characterId,
				bool enableThinkingSpeech,
				std::vector<CharacterLoadedVoiceData> voiceOverrideConfigs) :
			m_characterId(characterId),
			m_enableThinkingSpeech(enableThinkingSpeech),
			m_voiceData(voiceOverrideConfigs)
		{
		}
	};
}