// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaApiHandler.h"
#include "DataTypes/VoxtaResponseBase.h"
#include "DataTypes/VoxtaResponseWelcome.h"
#include "DataTypes/VoxtaResponseCharacterList.h"
#include <signalrclient/signalr_value.h>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "DataTypes/CharData.h"
#include <type_traits>
#include <format>
#include <stdexcept>
#include <cassert>

namespace Voxta
{
	VoxtaApiHandler::VoxtaApiHandler() :
		m_authenticateReqData(std::make_unique<signalr::value>(ConstructAuthenticateReqData())),
		m_loadCharacterListReqData(std::make_unique<signalr::value>(ConstructLoadCharacterListReqData()))
	{
	}

	signalr::value VoxtaApiHandler::GetRequestData(const VoxtaRequestType commData) const
	{
		return GetRequestData(commData, nullptr);
	}

	template<typename... Args>
	signalr::value VoxtaApiHandler::GetRequestData(const VoxtaRequestType commData, Args... args) const
	{
		switch (commData)
		{
			using enum Voxta::VoxtaApiHandler::VoxtaRequestType;
			case AUTHENTICATE:
				return *m_authenticateReqData.get();
			case LOAD_CHARACTERS_LIST:
				return *m_loadCharacterListReqData.get();
			case LOAD_CHARACTER:
				return ConstructLoadCharacterReqData(args...);
			default:
				assert(false && "Voxta::VoxtaApiHandler::GetRequestData is missing an implementation");
				break;
		}
	}

	signalr::value VoxtaApiHandler::ConstructLoadCharacterReqData(std::string_view characterId) const
	{
		return signalr::value(std::map<std::string, signalr::value> {
			{ "$type", "loadCharacter" },
			{ "characterId", characterId.data() }
		});
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
		else
		{
			throw std::invalid_argument(std::format("Voxta::VoxtaApiHandler::GetResponseData has no support for {}", type));
		}
	}
}