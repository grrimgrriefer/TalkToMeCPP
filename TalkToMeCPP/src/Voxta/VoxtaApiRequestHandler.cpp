// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Utility/GuidUtility.h"
#include "DataTypes/CharData.h"
#include "VoxtaApiRequestHandler.h"
#include <map>
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>

namespace Voxta
{
	signalr::value VoxtaApiRequestHandler::GetAuthenticateRequestData() const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "authenticate" },
			{ "client", "TalkToMeCPP" },
			{ "clientVersion", "0.0.2a" },
			{ "scope", std::vector<signalr::value> { "role:app", "broadcast:write" } },
			{ "capabilities", std::map<std::string, signalr::value> {
				{ "audioInput", "WebSocketStream" },
				{ "audioOutput", "Url" },
				{ "acceptedAudioContentTypes", std::vector<signalr::value> { "audio/x-wav" } }
			} }
		});
	}

	signalr::value VoxtaApiRequestHandler::GetLoadCharactersListData() const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "loadCharactersList" }
		});
	}

	signalr::value VoxtaApiRequestHandler::GetLoadCharacterRequestData(std::string_view characterId) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "loadCharacter" },
			{ "characterId", characterId.data() }
		});
	}

	signalr::value VoxtaApiRequestHandler::GetStartChatRequestData(const DataTypes::CharData* charData) const
	{
		std::string guidString = Utility::GuidUtility::GenerateGuid();

		auto characterParams = std::map<std::string, signalr::value>{
			{ "id", charData->m_id },
			{ "name", charData->m_name },
			{ "explicitContent", charData->m_explicitContent ? "True" : "False" } };

		if (charData->m_voiceService)
		{
			auto const& serviceData = *(charData->m_voiceService.get());
			characterParams.try_emplace("textToSpeech", std::vector<signalr::value> {
				std::map<std::string, signalr::value> {
					{ "voice", std::map<std::string, signalr::value> {
						{ "parameters", std::map<std::string, signalr::value> {
							{ "Filename", serviceData.m_parameters.m_filename },
							{ "Temperature", serviceData.m_parameters.m_temperature },
							{ "TopK", serviceData.m_parameters.m_topK },
							{ "TopP", serviceData.m_parameters.m_topP } } },
						{ "label", serviceData.m_parameters.m_filename }
					} },
					{ "service", std::map<std::string, signalr::value> {
						{ "serviceName", serviceData.m_name },
						{ "serviceId", serviceData.m_id }	} }
				} });
		}

		return signalr::value(std::map<std::string, signalr::value>{
			{ "$type", "startChat" },
			{ "contextKey", "" },
			{ "context", "" },
			{ "chatId", guidString },
			{ "characterId", charData->m_id },
			{ "character", characterParams }
		});
	}

	signalr::value VoxtaApiRequestHandler::GetSendUserMessageData(std::string_view sessionId,
		std::string_view userMessage) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "send" },
			{ "sessionId", sessionId.data() },
			{ "text", userMessage.data() },
			{ "doReply", "true" },
			{ "doCharacterActionInference", "false" }
		});
	}

	signalr::value VoxtaApiRequestHandler::GetNotifyAudioPlaybackStartData(std::string_view sessionId,
		std::string_view messageId,
		int startIndex,
		int endIndex,
		double duration) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "speechPlaybackStart" },
			{ "sessionId", sessionId.data() },
			{ "messageId", messageId.data() },
			{ "startIndex", signalr::value(static_cast<double>(startIndex)) },
			{ "endIndex", signalr::value(static_cast<double>(endIndex)) },
			{ "duration", signalr::value(duration) }
		});
	}

	signalr::value VoxtaApiRequestHandler::GetNotifyAudioPlaybackCompleteData(std::string_view sessionId,
		std::string_view messageId) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "speechPlaybackComplete" },
			{ "sessionId", sessionId.data() },
			{ "messageId", messageId.data() }
		});
	}
}