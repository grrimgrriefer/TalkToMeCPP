// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Utility/GuidUtility.h"
#include "DataTypes/CharData.h"
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "DataTypes/ServerResponses/ServerResponseChatMessage.h"
#include "DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include "DataTypes/ServerResponses/ServerResponseChatUpdate.h"
#include "DataTypes/ServerResponses/ServerResponseSpeechTranscription.h"
#include "DataTypes/ServerResponses/ServerResponseWelcome.h"
#include "DataTypes/ServiceData.h"
#include "VoxtaApiHandler.h"
#include <map>
#include <memory>
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>

namespace Voxta
{
	std::unique_ptr<DataTypes::ServerResponses::ServerResponseBase> VoxtaApiHandler::GetResponseData(
		const std::map<std::string, signalr::value>& map) const
	{
		using enum DataTypes::ServerResponses::ServerResponseType;
		std::string type = map.at("$type").as_string();
		if (type == "welcome")
		{
			return GetWelcomeResponse(map);
		}
		else if (type == "charactersListLoaded")
		{
			return GetCharacterListLoadedResponse(map);
		}
		else if (type == "characterLoaded")
		{
			return GetCharacterLoadedResponse(map);
		}
		else if (type == "chatStarted")
		{
			return GetChatStartedResponse(map);
		}
		else if (type == "replyStart")
		{
			return GetReplyStartReponseResponse(map);
		}
		else if (type == "replyChunk")
		{
			return GetReplyChunkReponseResponse(map);
		}
		else if (type == "replyEnd")
		{
			return GetReplyEndReponseResponse(map);
		}
		else if (type == "update")
		{
			return GetChatUpdateResponse(map);
		}
		else if (type == "speechRecognitionPartial")
		{
			return GetSpeechRecognitionPartial(map);
		}
		else if (type == "speechRecognitionEnd")
		{
			return GetSpeechRecognitionEnd(map);
		}
		// TODO: maybe?
		//		else if (type == "chatClosed") {}
		//		else if (type == "chatInProgress") {}
		//		else if (type == "chatSessionError") {}
		else
		{
			return nullptr;
		}
	}

	signalr::value VoxtaApiHandler::GetAuthenticateRequestData() const
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

	signalr::value VoxtaApiHandler::GetLoadCharactersListData() const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "loadCharactersList" }
		});
	}

	signalr::value VoxtaApiHandler::GetLoadCharacterRequestData(std::string_view characterId) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "loadCharacter" },
			{ "characterId", characterId.data() }
		});
	}

	signalr::value VoxtaApiHandler::GetStartChatRequestData(const DataTypes::CharData* charData) const
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

	signalr::value VoxtaApiHandler::GetSendUserMessageData(std::string_view sessionId, std::string_view userMessage) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "send" },
			{ "sessionId", sessionId.data() },
			{ "text", userMessage.data() },
			{ "doReply", "true" },
			{ "doCharacterActionInference", "false" }
		});
	}

	signalr::value VoxtaApiHandler::GetNotifyAudioPlaybackStartData(std::string_view sessionId, std::string_view messageId,
		int startIndex, int endIndex, double duration) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "speechPlaybackStart" },
			{ "sessionId", sessionId.data() },
			{ "messageId", messageId.data() },
			{ "startIndex", signalr::value(static_cast<double>(startIndex)) },
			{ "endIndex", signalr::value(static_cast<double>(endIndex)) },
			{ "duration", signalr::value(static_cast<double>(duration)) }
		});
	}

	signalr::value VoxtaApiHandler::GetNotifyAudioPlaybackCompleteData(std::string_view sessionId, std::string_view messageId) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "speechPlaybackComplete" },
			{ "sessionId", sessionId.data() },
			{ "messageId", messageId.data() }
		});
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatUpdate> VoxtaApiHandler::GetChatUpdateResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		return std::make_unique<DataTypes::ServerResponses::ServerResponseChatUpdate>(
			map.at("messageId").as_string(),
			map.at("senderId").as_string(),
			map.at("text").as_string(),
			map.at("sessionId").as_string());
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseSpeechTranscription> VoxtaApiHandler::GetSpeechRecognitionPartial(
		const std::map<std::string, signalr::value>& map) const
	{
		return std::make_unique<DataTypes::ServerResponses::ServerResponseSpeechTranscription>(
			map.at("text").as_string(), Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::PARTIAL);
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseSpeechTranscription> VoxtaApiHandler::GetSpeechRecognitionEnd(
		const std::map<std::string, signalr::value>& map) const
	{
		return std::make_unique<DataTypes::ServerResponses::ServerResponseSpeechTranscription>(
			map.contains("text") ? map.at("text").as_string() : "",
			map.contains("text") ? Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::END :
			Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::CANCELLED);
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatMessage> VoxtaApiHandler::GetReplyEndReponseResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		return std::make_unique<DataTypes::ServerResponses::ServerResponseChatMessage>(
			DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_END,
			map.at("messageId").as_string(),
			map.at("senderId").as_string(),
			map.at("sessionId").as_string());
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatMessage> VoxtaApiHandler::GetReplyChunkReponseResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		return std::make_unique<DataTypes::ServerResponses::ServerResponseChatMessage>(
			DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_CHUNK,
			map.at("messageId").as_string(),
			map.at("senderId").as_string(),
			map.at("sessionId").as_string(),
			static_cast<int>(map.at("startIndex").as_double()),
			static_cast<int>(map.at("endIndex").as_double()),
			map.at("text").as_string(),
			map.at("audioUrl").as_string());
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatMessage> VoxtaApiHandler::GetReplyStartReponseResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		return std::make_unique<DataTypes::ServerResponses::ServerResponseChatMessage>(
			DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_START,
			map.at("messageId").as_string(),
			map.at("senderId").as_string(),
			map.at("sessionId").as_string());
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatStarted> VoxtaApiHandler::GetChatStartedResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		auto& user = map.at("user").as_map();
		auto& charIdArray = map.at("characters").as_array();
		std::vector<std::string> chars;
		chars.reserve(charIdArray.size());
		for (auto& charElement : charIdArray)
		{
			auto& characterData = charElement.as_map();
			chars.push_back(characterData.at("id").as_string());
		}

		auto& servicesMap = map.at("services").as_map();
		using enum DataTypes::ServiceData::ServiceType;
		std::map<const DataTypes::ServiceData::ServiceType, const DataTypes::ServiceData> services;
		std::map<DataTypes::ServiceData::ServiceType, std::string> serviceTypes = {
			{ TEXT_GEN, "textGen" },
			{ SPEECH_TO_TEXT, "speechToText" },
			{ TEXT_TO_SPEECH, "textToSpeech" }
		};

		for (const auto& [enumType, stringValue] : serviceTypes)
		{
			if (servicesMap.contains(stringValue))
			{
				auto& serviceData = servicesMap.at(stringValue).as_map();
				services.try_emplace(enumType, enumType, serviceData.at("serviceName").as_string(),
					serviceData.at("serviceId").as_string());
			}
		}
		return std::make_unique<DataTypes::ServerResponses::ServerResponseChatStarted>(user.at("id").as_string(),
			chars, services, map.at("chatId").as_string(), map.at("sessionId").as_string());
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseCharacterLoaded> VoxtaApiHandler::GetCharacterLoadedResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		auto& characterData = map.at("character").as_map();
		auto& ttsConfig = characterData.at("textToSpeech").as_array();
		auto configs = std::vector<DataTypes::ServerResponses::CharacterLoadedVoiceData>();

		for (const auto& config : ttsConfig)
		{
			auto& configMap = config.as_map();
			auto& configVoice = configMap.at("voice").as_map();
			auto& originalParamsMap = configVoice.at("parameters").as_map();
			std::map<std::string, std::string> converted_map;
			for (const auto& pair : originalParamsMap)
			{
				converted_map[pair.first] = pair.second.as_string();
			}
			configs.emplace_back(converted_map, configVoice.contains("label")
				? configVoice.at("label").as_string() : nullptr);
		}
		return std::make_unique<DataTypes::ServerResponses::ServerResponseCharacterLoaded>(characterData.at("id").as_string(),
			characterData.at("enableThinkingSpeech").as_bool(), configs);
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseCharacterList> VoxtaApiHandler::GetCharacterListLoadedResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		auto& charArray = map.at("characters").as_array();
		std::vector<DataTypes::CharData> chars;
		chars.reserve(charArray.size());
		for (auto& charElement : charArray)
		{
			auto& characterData = charElement.as_map();
			auto character = DataTypes::CharData(characterData.at("id").as_string(), characterData.at("name").as_string());
			character.m_creatorNotes = characterData.contains("creatorNotes") ? characterData.at("creatorNotes").as_string() : "";
			character.m_explicitContent = characterData.contains("explicitContent") ? characterData.at("explicitContent").as_bool() : false;
			character.m_favorite = characterData.contains("favorite") ? characterData.at("favorite").as_bool() : false;
			chars.push_back(character);
		}
		return std::make_unique<DataTypes::ServerResponses::ServerResponseCharacterList>(chars);
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseWelcome> VoxtaApiHandler::GetWelcomeResponse(
		const std::map<std::string, signalr::value>& map) const
	{
		auto& user = map.at("user").as_map();
		return std::make_unique<DataTypes::ServerResponses::ServerResponseWelcome>(
			DataTypes::CharData(user.at("id").as_string(), user.at("name").as_string()));
	}
}