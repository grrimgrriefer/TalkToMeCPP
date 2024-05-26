// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "pch.h"
#include "gmock/gmock.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiHandler.h"
#include "../TalkToMeCPP/src/Utility/Logging/LoggerInterface.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ChatMessage.h"
#include "../TalkToMeCPP/src/Utility/SignalR/SignalRWrapperInterface.h"
#include <functional>
#include <string>
#include <gmock/gmock-function-mocker.h>
#include <gmock/gmock-spec-builders.h>
#include <gmock/gmock-actions.h>
#include <signalrclient/connection_state.h>
#include <memory>
#include <type_traits>
#include <exception>
#include <map>
#include <vector>
#include <gmock/gmock-matchers.h>
#include <signalrclient/signalr_value.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	class MockProviders
	{
	public:
		class MockHubConnection : public Utility::SignalR::SignalRWrapperInterface
		{
		public:
			MOCK_METHOD(void, Start, (std::function<void(std::exception_ptr)>), (noexcept, override));
			MOCK_METHOD(void, Stop, (std::function<void(std::exception_ptr)>), (noexcept, override));
			MOCK_METHOD(void, On, (const std::string&, const std::function<void(const std::vector<signalr::value>&)>&), (override));
			MOCK_METHOD(void, Invoke, (const std::string&, const std::vector<signalr::value>&, std::function<void(const signalr::value&, std::exception_ptr)>), (noexcept, override));
		};

		class MockLogger : public Utility::Logging::LoggerInterface
		{
		public:
			MOCK_METHOD(void, LogMessage, (LogLevel level, const std::string& message), (noexcept, override));
		};

		static std::string GetRandomGuid()
		{
			boost::uuids::uuid guid = boost::uuids::random_generator()();
			return boost::lexical_cast<std::string>(guid);
		}

		static signalr::value GetCharactersListLoadedResponse(const std::vector<std::string_view>& characterIds, const std::vector<std::string_view>& characterNames,
			const std::vector<std::string_view>& creatorNotes, const std::vector<bool>& isExplicit, const std::vector<bool>& favorite)
		{
			Assert::AreEqual(characterIds.size(), characterNames.size());
			Assert::AreEqual(characterIds.size(), creatorNotes.size());
			Assert::AreEqual(characterIds.size(), isExplicit.size());
			Assert::AreEqual(characterIds.size(), favorite.size());

			std::vector<signalr::value> chars;
			for (int i = 0; i < characterIds.size(); i++)
			{
				chars.emplace_back(std::map<std::string, signalr::value> {
					{ "id", signalr::value(characterIds[i].data()) },
					{ "name", signalr::value(characterNames[i].data()) },
					{ "creatorNotes", signalr::value(creatorNotes[i].data()) },
					{ "explicitContent", signalr::value(isExplicit[i]) },
					{ "favorite", signalr::value(favorite[i]) } });
			}

			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "charactersListLoaded" },
				{ "characters", chars }
			});
		}

		static signalr::value GetCharacterLoadedResponse(const std::string_view& characterId, const bool enableThinkingSpeech)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "characterLoaded" },
				{ "character", std::map<std::string, signalr::value> {
					{ "id", signalr::value(characterId.data()) },
					{ "textToSpeech", std::vector<signalr::value> {
						std::map<std::string, signalr::value> {
							{ "voice", std::map<std::string, signalr::value> {
								{ "parameters", std::map<std::string, signalr::value> {
									{ "VoiceBackend", "unrealspeech" },
									{ "VoiceId", "Liv" }
								} },
								{ "label", "Liv" }
							} },
							{ "service", std::map<std::string, signalr::value> {
								{ "serviceName", "VoxtaCloud" }
							} }
						}
					}},
					{ "enableThinkingSpeech", enableThinkingSpeech } } }
			});
		}

		static signalr::value GetWelcomeResponse(const std::string_view& userId, const std::string_view& userName)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "welcome" },
				{ "user", std::map<std::string, signalr::value> {
					{ "id", signalr::value(userId.data()) },
					{ "name",  signalr::value(userName.data()) } } }
			});
		}

		static signalr::value GetStartChatResponse(const std::string_view& characterId, const std::string_view& chatId)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "startChat" },
				{ "character", std::map<std::string, signalr::value> {
					{ "id", signalr::value(characterId.data()) } } },
				{ "characterId", signalr::value(characterId.data()) },
				{ "chatId", signalr::value(chatId.data()) }
			});
		}

		static signalr::value GetChatStartedResponse(const std::string_view& chatId, const std::string_view& userId,
			const std::string_view& userName, const std::string_view& llmServiceId, const std::string_view& sttServiceId,
			const std::string_view& ttsServiceId, const std::string_view& characterId, const std::string_view& characterName,
			const std::string_view& sessionId)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "chatStarted" },
				{ "chatId", signalr::value(chatId.data()) },
				{ "user", std::map<std::string, signalr::value> {
					{ "id", signalr::value(userId.data()) },
					{ "name", signalr::value(userName.data()) }
				} },
				{ "services", std::map<std::string, signalr::value> {
					{ "textGen", std::map<std::string, signalr::value> {
						{ "serviceName", "VoxtaCloud" },
						{ "serviceId", signalr::value(llmServiceId.data()) } } },
					{ "speechToText", std::map<std::string, signalr::value> {
						{ "serviceName", "VoxtaCloud" },
						{ "serviceId", signalr::value(sttServiceId.data()) } } },
					{ "textToSpeech", std::map<std::string, signalr::value> {
						{ "serviceName", "Coqui" },
						{ "serviceId", signalr::value(ttsServiceId.data()) } } }
				} },
				{ "characters", std::vector<signalr::value> {
					std::map<std::string, signalr::value> {
						{ "id", signalr::value(characterId.data()) },
						{ "name", signalr::value(characterName.data()) }
					}
					} },
				{ "sessionId", signalr::value(sessionId.data()) }
			});
		}

		static signalr::value GetReplyStartResponse(const std::string_view& messageId1, const std::string_view& id1,
			const std::string_view& sessionId)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "replyStart" },
				{ "messageId", signalr::value(messageId1.data()) },
				{ "senderId", signalr::value(id1.data()) },
				{ "sessionId", signalr::value(sessionId.data()) }
			});
		}

		static signalr::value GetReplyChunkResponse(const std::string_view& messageId1, const std::string_view& id1,
			double startIndex, double endIndex, const std::string_view& messageText1, const std::string_view& audioUrl1,
			const std::string_view& sessionId)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "replyChunk" },
				{ "messageId", signalr::value(messageId1.data()) },
				{ "senderId", signalr::value(id1.data()) },
				{ "startIndex", startIndex },
				{ "endIndex", endIndex },
				{ "text", signalr::value(messageText1.data()) },
				{ "audioUrl", signalr::value(audioUrl1.data()) },
				{ "sessionId", signalr::value(sessionId.data()) }
			});
		}

		static signalr::value GetReplyEndResponse(const std::string_view& messageId1, const std::string_view& id1,
			const std::string_view& sessionId)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "replyEnd" },
				{ "messageId", signalr::value(messageId1.data()) },
				{ "senderId", signalr::value(id1.data()) },
				{ "sessionId", signalr::value(sessionId.data()) }
			});
		}

		static signalr::value GetUpdateChatResponse(const std::string_view& messageId1, const std::string_view& id1,
			const std::string_view& text, const std::string_view& sessionId)
		{
			return signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "update" },
				{ "messageId", signalr::value(messageId1.data()) },
				{ "senderId", signalr::value(id1.data()) },
				{ "text", signalr::value(text.data()) },
				{ "sessionId", signalr::value(sessionId.data()) }
			});
		}
	};
}