// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "pch.h"
#include "gmock/gmock.h"
#include "CppUnitTest.h"
#include "../MockProviders.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiRequestHandler.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"
#include "../TalkToMeCPP/src/Utility/GuidUtility.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(VoxtaApiRequestHandlerTests)
	{
	public:
		TEST_METHOD(TestGetAuthenticateRequestData)
		{
			auto response = Voxta::VoxtaApiRequestHandler().GetAuthenticateRequestData();

			Assert::IsTrue(response.is_map());
			auto& responseMap = response.as_map();
			Assert::AreEqual(std::string("authenticate"), responseMap.at("$type").as_string());
			Assert::AreEqual(std::string("TalkToMeCPP"), responseMap.at("client").as_string());

			Assert::IsTrue(responseMap.at("scope").is_array());
			auto& scopeArray = responseMap.at("scope").as_array();
			Assert::AreEqual(std::string("role:app"), scopeArray[0].as_string());
			Assert::AreEqual(std::string("broadcast:write"), scopeArray[1].as_string());

			Assert::IsTrue(responseMap.at("capabilities").is_map());
			auto& capabilities = responseMap.at("capabilities").as_map();
			Assert::AreEqual(std::string("WebSocketStream"), capabilities.at("audioInput").as_string());
			Assert::AreEqual(std::string("Url"), capabilities.at("audioOutput").as_string());

			Assert::IsTrue(capabilities.at("acceptedAudioContentTypes").is_array());
			auto& acceptedAudioContentTypes = capabilities.at("acceptedAudioContentTypes").as_array();
			Assert::AreEqual(size_t(1), acceptedAudioContentTypes.size());
			Assert::AreEqual(std::string("audio/x-wav"), acceptedAudioContentTypes[0].as_string());
		}

		TEST_METHOD(TestGetLoadCharactersListData)
		{
			auto response = Voxta::VoxtaApiRequestHandler().GetLoadCharactersListData();

			Assert::IsTrue(response.is_map());
			auto& responseMap = response.as_map();
			Assert::AreEqual(std::string("loadCharactersList"), responseMap.at("$type").as_string());
		}

		TEST_METHOD(TestGetLoadCharacterRequestData)
		{
			auto guid = Utility::GuidUtility::GenerateGuid();
			auto response = Voxta::VoxtaApiRequestHandler().GetLoadCharacterRequestData(guid);

			Assert::IsTrue(response.is_map());
			auto& responseMap = response.as_map();
			Assert::AreEqual(std::string("loadCharacter"), responseMap.at("$type").as_string());
			Assert::AreEqual(guid, responseMap.at("characterId").as_string());
		}

		TEST_METHOD(TestGetStartChatRequestData)
		{
			const auto charData = Voxta::DataTypes::CharData(Utility::GuidUtility::GenerateGuid(), "HuhWeird");
			auto response = Voxta::VoxtaApiRequestHandler().GetStartChatRequestData(&charData);

			Assert::IsTrue(response.is_map());
			auto& responseMap = response.as_map();
			Assert::AreEqual(std::string("startChat"), responseMap.at("$type").as_string());

			Assert::AreNotEqual(std::string(""), responseMap.at("chatId").as_string());
			Assert::AreEqual(Utility::GuidUtility::GenerateGuid().size(), responseMap.at("chatId").as_string().size());

			Assert::AreEqual(charData.m_id, responseMap.at("characterId").as_string());

			Assert::IsTrue(responseMap.at("character").is_map());
			auto& characterMap = responseMap.at("character").as_map();

			Assert::AreEqual(charData.m_id, characterMap.at("id").as_string());
			Assert::AreEqual(charData.m_name, characterMap.at("name").as_string());
			Assert::AreEqual(std::string(charData.m_explicitContent ? "True" : "False"), characterMap.at("explicitContent").as_string()); // string value of bool is intentional

			// Todo: assertions for TTS settings
		}

		TEST_METHOD(TestGetSendUserMessageData)
		{
			auto sessionId = Utility::GuidUtility::GenerateGuid();
			std::string message = "justsometestmessage hola";
			auto response = Voxta::VoxtaApiRequestHandler().GetSendUserMessageData(sessionId, message);

			Assert::IsTrue(response.is_map());
			auto& responseMap = response.as_map();
			Assert::AreEqual(std::string("send"), responseMap.at("$type").as_string());
			Assert::AreEqual(sessionId, responseMap.at("sessionId").as_string());
			Assert::AreEqual(message, responseMap.at("text").as_string());
			Assert::AreEqual(std::string("true"), responseMap.at("doReply").as_string()); // string value of bool is intentional
			Assert::AreEqual(std::string("false"), responseMap.at("doCharacterActionInference").as_string()); // string value of bool is intentional
		}
	};
}