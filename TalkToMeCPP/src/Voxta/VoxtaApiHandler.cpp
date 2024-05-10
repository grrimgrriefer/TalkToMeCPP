// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaApiHandler.h"
#include "DataTypes/CharData.h"
#include "DataTypes/ServiceData.h"
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "DataTypes/ServerResponses/ServerResponseWelcome.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include <signalrclient/signalr_value.h>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <format>
#include <stdexcept>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#pragma comment(lib, "bcrypt.lib")

namespace Voxta
{
	VoxtaApiHandler::VoxtaApiHandler() :
		m_authenticateReqData(std::make_unique<signalr::value>(ConstructAuthenticateReqData())),
		m_loadCharacterListReqData(std::make_unique<signalr::value>(ConstructLoadCharacterListReqData()))
	{
	}

	signalr::value VoxtaApiHandler::GetRequestData(const VoxtaGenericRequestType commData) const
	{
		switch (commData)
		{
			using enum Voxta::VoxtaApiHandler::VoxtaGenericRequestType;
			case AUTHENTICATE:
				return *m_authenticateReqData.get();
			case LOAD_CHARACTERS_LIST:
				return *m_loadCharacterListReqData.get();
		}
		return nullptr;
	}

	signalr::value VoxtaApiHandler::ConstructAuthenticateReqData() const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "authenticate" },
			{ "client", "TalkToMeCPP" },
			{ "clientVersion", "0.0.1a" },
			{ "scope", std::vector<signalr::value> { "role:app" } },
			{ "capabilities", std::map<std::string, signalr::value> {
				{ "audioInput", "WebSocketStream" },
				{ "audioOutput", "Url" },
				{ "acceptedAudioContentTypes", std::vector<signalr::value> { "audio/x-wav" } }
			} }
		});
	}

	signalr::value VoxtaApiHandler::ConstructLoadCharacterListReqData() const
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

	signalr::value VoxtaApiHandler::GetStartChatRequestData(std::shared_ptr<DataTypes::CharData>& charData) const
	{
		boost::uuids::uuid guid = boost::uuids::random_generator()();
		std::string guidString = boost::lexical_cast<std::string>(guid);

		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "startChat" },
			{ "contextKey", "" },
			{ "context", "" },
			{ "chatId", guidString },
			{ "characterId", charData->m_id },
			{ "character", std::map<std::string, signalr::value> {
					{ "id", charData->m_id },
					{ "name", charData->m_name },
					{ "explicitContent", charData->m_explicitContent ? "True" : "False" },
					{ "textToSpeech", std::vector<signalr::value> {
						std::map<std::string, signalr::value> {
							{ "voice", std::map<std::string, signalr::value> {
								{ "parameters", std::map<std::string, signalr::value> {
									{ "Filename", "female_03.wav" },
									{ "Temperature", "0.7"},
									{ "TopK", signalr::value() },
									{ "TopP", signalr::value() } } },
								{ "label", "female_03.wav" }
							} },
							{ "service", std::map<std::string, signalr::value> {
								{ "serviceName", "Coqui"},
								{ "serviceId", "aef6d791-314f-f1e7-32f1-72b382dc7bd9" }	} }
					} } } }
			}
		});
	}

	std::unique_ptr<DataTypes::ServerResponses::ServerResponseBase> VoxtaApiHandler::GetResponseData(
		const std::map<std::string, signalr::value>& map) const
	{
		using enum DataTypes::ServerResponses::ServerResponseType;
		std::string type = map.at("$type").as_string();
		if (type == "welcome")
		{
			auto& user = map.at("user").as_map();
			return std::make_unique<DataTypes::ServerResponses::ServerResponseWelcome>(
				DataTypes::CharData(user.at("id").as_string(), user.at("name").as_string()));
		}
		else if (type == "charactersListLoaded")
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
		else if (type == "characterLoaded")
		{
			auto& characterData = map.at("character").as_map();
			return std::make_unique<DataTypes::ServerResponses::ServerResponseCharacterLoaded>(characterData.at("id").as_string(),
				characterData.at("enableThinkingSpeech").as_bool());
		}
		else if (type == "chatStarted")
		{
			auto& user = map.at("user").as_map();
			auto& charIdArray = map.at("characters").as_array();
			std::vector<std::string_view> chars;
			chars.reserve(charIdArray.size());
			for (auto& charElement : charIdArray)
			{
				auto& characterData = charElement.as_map();
				chars.push_back(characterData.at("id").as_string());
			}

			auto& servicesMap = map.at("services").as_map();
			using enum DataTypes::ServiceData::ServiceType;
			std::map<DataTypes::ServiceData::ServiceType, DataTypes::ServiceData> services;
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
					services.try_emplace(enumType, DataTypes::ServiceData(enumType, serviceData.at("serviceName").as_string(),
						serviceData.at("serviceId").as_string()));
				}
			}
			return std::make_unique<DataTypes::ServerResponses::ServerResponseChatStarted>(user.at("id").as_string(),
				chars, services, map.at("chatId").as_string(), map.at("sessionId").as_string());
		}
		// TODO:
		//		else if (type == "replyStart") {}
		//		else if (type == "replyChunk") {}
		//		else if (type == "replyEnd") {}
		//		else if (type == "replyEnd") {}
		//		else if (type == "chatClosed") {}
		//		else if (type == "chatInProgress") {}
		//		else if (type == "chatSessionError") {}
		return nullptr;
	}
}