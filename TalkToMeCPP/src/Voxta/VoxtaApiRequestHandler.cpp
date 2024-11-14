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
			{ "clientVersion", "0.0.3a" },
			{ "scope", std::vector<signalr::value> { "role:app" } },
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

	signalr::value VoxtaApiRequestHandler::GetLoadScenariosListData() const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "loadScenariosList" }
		});
	}

	signalr::value VoxtaApiRequestHandler::GeLoadChatsListData(std::string_view scenarioId, std::string_view characterId) const
	{
		std::map<std::string, signalr::value> returnValue = std::map<std::string, signalr::value>{
			{ "$type", "loadChatsList" },
			{ "characterId", characterId.data() }
		};
		if (!scenarioId.empty())
		{
			returnValue.try_emplace("scenarioId", signalr::value(scenarioId.data()));
		}
		return signalr::value(returnValue);
	}

	signalr::value VoxtaApiRequestHandler::GetStartChatRequestData(const DataTypes::CharData* charData) const
	{
		return signalr::value(std::map<std::string, signalr::value>{
			{ "$type", "startChat" },
			{ "contextKey", "" },
			{ "characterIds", std::vector<signalr::value> { charData->m_id } },
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