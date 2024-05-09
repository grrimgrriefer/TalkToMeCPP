// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaApiHandler.h"
#include "DataTypes/CharData.h"
#include "DataTypes/VoxtaResponseBase.h"
#include "DataTypes/VoxtaResponseWelcome.h"
#include "DataTypes/VoxtaResponseCharacterList.h"
#include "DataTypes/VoxtaResponseCharacterLoaded.h"
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

	signalr::value VoxtaApiHandler::GetStartChatRequestData(DataTypes::CharData charData) const
	{
		boost::uuids::uuid guid = boost::uuids::random_generator()();
		std::string guidString = boost::lexical_cast<std::string>(guid);

		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "startChat" },
			{ "contextKey", "" },
			{ "context", "" },
			{ "chatId", guidString },
			{ "characterId", charData.m_id },
			{ "character", std::map<std::string, signalr::value> {
					{ "id", charData.m_id },
					{ "name", charData.m_name },
					{ "explicitContent", charData.m_explicitContent ? "True" : "False" },
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

	std::unique_ptr<DataTypes::VoxtaResponseBase> VoxtaApiHandler::GetResponseData(
		const std::map<std::string, signalr::value>& map) const
	{
		using enum DataTypes::VoxtaResponseType;
		std::string type = map.at("$type").as_string();
		if (type == "welcome")
		{
			auto& user = map.at("user").as_map();
			return std::make_unique<DataTypes::VoxtaResponseWelcome>(
				DataTypes::CharData(user.at("id").as_string(), user.at("name").as_string()));
		}
		else if (type == "charactersListLoaded")
		{
			auto& charArray = map.at("characters").as_array();
			std::vector<DataTypes::CharData> chars;
			chars.reserve(charArray.size());
			for (int i = 0; i < charArray.size(); i++)
			{
				auto& characterData = charArray[i].as_map();
				auto character = DataTypes::CharData(characterData.at("id").as_string(), characterData.at("name").as_string());
				character.m_creatorNotes = characterData.contains("creatorNotes") ? characterData.at("creatorNotes").as_string() : "";
				character.m_explicitContent = characterData.contains("explicitContent") ? characterData.at("explicitContent").as_bool() : false;
				character.m_favorite = characterData.contains("favorite") ? characterData.at("favorite").as_bool() : false;
				chars.push_back(character);
			}
			return std::make_unique<DataTypes::VoxtaResponseCharacterList>(chars);
		}
		else if (type == "characterLoaded")
		{
			auto& characterData = map.at("character").as_map();
			return std::make_unique<DataTypes::VoxtaResponseCharacterLoaded>(characterData.at("id").as_string(),
				characterData.at("enableThinkingSpeech").as_bool());
		}
		else
		{
			throw std::invalid_argument(std::format("Voxta::VoxtaApiHandler::GetResponseData has no support for {}", type));
		}
	}
}