// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "pch.h"
#include "gmock/gmock.h"
#include "CppUnitTest.h"
#include "../MockProviders.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiResponseHandler.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseChatMessage.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseChatUpdate.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseWelcome.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseSpeechTranscription.h"
#include "../TalkToMeCPP/src/Utility/GuidUtility.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(VoxtaApiReponseHandlerTests)
	{
	public:
		TEST_METHOD(TestGetResponseDataGetServerResponseWelcome)
		{
			std::string userName("waitwat?");
			std::string userId(Utility::GuidUtility::GenerateGuid());
			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetWelcomeResponse(userId, userName).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseWelcome*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(userName, derivedResponse->m_user.m_name);
			Assert::AreEqual(userId, derivedResponse->m_user.m_id);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseCharacterList)
		{
			std::string id1 = Utility::GuidUtility::GenerateGuid(), id2 = Utility::GuidUtility::GenerateGuid();
			std::string name1 = "Bella", name2 = "Catherine";
			std::string creatorNotes1 = "Magnetic woman", creatorNotes2 = "Cute and flirty";
			bool explicitContent1 = true, explicitContent2 = false;
			bool favorite1 = false, favorite2 = false;

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetCharactersListLoadedResponse({ id1 , id2 }, { name1 , name2 },
				{ creatorNotes1 , creatorNotes2 }, { explicitContent1 , explicitContent2 }, { favorite1 , favorite2 }).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseCharacterList*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(size_t(2), derivedResponse->m_characters.size());
			Assert::AreEqual(id1, derivedResponse->m_characters[0].m_id);
			Assert::AreEqual(name1, derivedResponse->m_characters[0].m_name);
			Assert::AreEqual(creatorNotes1, derivedResponse->m_characters[0].m_creatorNotes);
			Assert::AreEqual(explicitContent1, derivedResponse->m_characters[0].m_explicitContent);
			Assert::AreEqual(favorite1, derivedResponse->m_characters[0].m_favorite);
			Assert::AreEqual(id2, derivedResponse->m_characters[1].m_id);
			Assert::AreEqual(name2, derivedResponse->m_characters[1].m_name);
			Assert::AreEqual(creatorNotes2, derivedResponse->m_characters[1].m_creatorNotes);
			Assert::AreEqual(explicitContent2, derivedResponse->m_characters[1].m_explicitContent);
			Assert::AreEqual(favorite2, derivedResponse->m_characters[1].m_favorite);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseCharacterLoaded)
		{
			std::string characterId(Utility::GuidUtility::GenerateGuid());
			bool enableThinkingSpeech(true);
			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetCharacterLoadedResponse(characterId, enableThinkingSpeech).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseCharacterLoaded*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(characterId, derivedResponse->m_characterId);
			Assert::AreEqual(enableThinkingSpeech, derivedResponse->m_enableThinkingSpeech);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseChatStarted)
		{
			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string chatId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string userId = Utility::GuidUtility::GenerateGuid();
			std::string llmServiceId = Utility::GuidUtility::GenerateGuid();
			std::string sttServiceId = Utility::GuidUtility::GenerateGuid();
			std::string ttsServiceId = Utility::GuidUtility::GenerateGuid();

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetChatStartedResponse(chatId, userId, "",
				llmServiceId, sttServiceId, ttsServiceId, charId, "", sessionId).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseChatStarted*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(size_t(1), derivedResponse->m_characterIds.size());
			Assert::AreEqual(charId, derivedResponse->m_characterIds[0]);
			Assert::AreEqual(chatId, derivedResponse->m_chatId);
			Assert::AreEqual(sessionId, derivedResponse->m_sessionId);
			Assert::AreEqual(userId, derivedResponse->m_userId);
			Assert::AreEqual(size_t(3), derivedResponse->m_services.size());
			Assert::AreEqual(llmServiceId, derivedResponse->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::TEXT_GEN).m_serviceId);
			Assert::AreEqual(sttServiceId, derivedResponse->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::SPEECH_TO_TEXT).m_serviceId);
			Assert::AreEqual(ttsServiceId, derivedResponse->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::TEXT_TO_SPEECH).m_serviceId);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseChatMessageStart)
		{
			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string messageId = Utility::GuidUtility::GenerateGuid();
			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetReplyStartResponse(messageId, charId, sessionId).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseChatMessage*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(charId, derivedResponse->m_senderId);
			Assert::AreEqual(sessionId, derivedResponse->m_sessionId);
			Assert::AreEqual(messageId, derivedResponse->m_messageId);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseChatMessageChunk)
		{
			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string messageId = Utility::GuidUtility::GenerateGuid();
			std::string messageText = "Lmao I aint leaking my ai messages on github xd";
			std::string audioUrl = "/api/tts/gens/etc...";

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetReplyChunkResponse(messageId, charId, static_cast<double>(0),
				static_cast<double>(47), messageText, audioUrl, sessionId).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseChatMessage*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(charId, derivedResponse->m_senderId);
			Assert::AreEqual(sessionId, derivedResponse->m_sessionId);
			Assert::AreEqual(messageId, derivedResponse->m_messageId);
			Assert::AreEqual(messageText, derivedResponse->m_messageText);
			Assert::AreEqual(audioUrl, derivedResponse->m_audioUrlPath);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseChatMessageEnd)
		{
			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string messageId = Utility::GuidUtility::GenerateGuid();

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetReplyEndResponse(messageId, charId, sessionId).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseChatMessage*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(charId, derivedResponse->m_senderId);
			Assert::AreEqual(sessionId, derivedResponse->m_sessionId);
			Assert::AreEqual(messageId, derivedResponse->m_messageId);
		}

		TEST_METHOD(TestGetResponseDataGetServerResponseChatUpdate)
		{
			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string text = "just some random message idk";
			std::string messageId = Utility::GuidUtility::GenerateGuid();

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetUpdateChatResponse(messageId, charId, text, sessionId).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseChatUpdate*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(charId, derivedResponse->m_senderId);
			Assert::AreEqual(sessionId, derivedResponse->m_sessionId);
			Assert::AreEqual(text, derivedResponse->m_text);
			Assert::AreEqual(messageId, derivedResponse->m_messageId);
		}

		TEST_METHOD(TryGetSpeechRecognitionPartialResponse)
		{
			std::string text = "just another random message idk";

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetSpeechRecognitionPartialResponse(text).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(text, derivedResponse->m_transcribedSpeech);
			Assert::AreEqual(static_cast<int>(Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::PARTIAL), static_cast<int>(derivedResponse->m_transcriptionState));
		}

		TEST_METHOD(TryGetSpeechRecognitionEndResponse)
		{
			std::string text = "I only keep making more random message idk";

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetSpeechRecognitionEndResponse(text, false).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::AreEqual(text, derivedResponse->m_transcribedSpeech);
			Assert::AreEqual(static_cast<int>(Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::END), static_cast<int>(derivedResponse->m_transcriptionState));
		}

		TEST_METHOD(TryGetSpeechRecognitionEndResponseCancelled)
		{
			std::string text = "I only keep making more random message idk";

			auto response = Voxta::VoxtaApiResponseHandler().GetResponseData(MockProviders::GetSpeechRecognitionEndResponse(text, true).as_map());
			auto derivedResponse = dynamic_cast<Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription*>(response.get());

			Assert::IsNotNull(derivedResponse);
			Assert::IsTrue(derivedResponse->m_transcribedSpeech.empty());
			Assert::AreEqual(static_cast<int>(Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::CANCELLED), static_cast<int>(derivedResponse->m_transcriptionState));
		}
	};
}