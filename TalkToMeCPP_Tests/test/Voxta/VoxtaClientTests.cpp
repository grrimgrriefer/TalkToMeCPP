// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "gmock/gmock.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiHandler.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiHandler.cpp"
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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VoxtaClientTests
{
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

	TEST_CLASS(VoxtaClientTests)
	{
	public:
		// Mocks for the callback functions
		Voxta::VoxtaClient::VoxtaClientState newState;
		std::string userInput;
		const Voxta::DataTypes::ChatMessage* lastChatMessage;
		const Voxta::DataTypes::CharData* lastCharData;

		// Mock implementations for the callback functions
		std::function<void(Voxta::VoxtaClient::VoxtaClientState)> stateChangeMock = [this] (Voxta::VoxtaClient::VoxtaClientState newStateParam)
			{
				newState = newStateParam;
			};
		std::function<std::string()> requestingUserInputEventMock = [this] ()
			{
				return userInput;
			};
		std::function<void(const Voxta::DataTypes::ChatMessage*, const Voxta::DataTypes::CharData*)> charSpeakingEventMock = [this] (const Voxta::DataTypes::ChatMessage* chatMsg, const Voxta::DataTypes::CharData* charData)
			{
				lastChatMessage = chatMsg;
				lastCharData = charData;
			};

		// Mocks for SignalR and ThreadedLogger
		std::unique_ptr<MockHubConnection> mockWrapper;
		std::unique_ptr<MockLogger> mockLogger;
		std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> signalRWrapper;
		std::unique_ptr<Utility::Logging::LoggerInterface> loggerWrapper;

		TEST_METHOD_INITIALIZE(MethodInitialize)
		{
			newState = Voxta::VoxtaClient::VoxtaClientState::DISCONNECTED;
			userInput = "";
			lastChatMessage = nullptr;
			lastCharData = nullptr;

			mockWrapper = std::make_unique<MockHubConnection>();
			mockLogger = std::make_unique<MockLogger>();
		}
		TEST_METHOD(TestConnectStartListening)
		{
			std::string methodName;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&methodName));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().Connect();
			Assert::AreEqual(std::string("ReceiveMessage"), methodName);
		}

		TEST_METHOD(TestConnectStartInvoked)
		{
			bool triggered = false;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().Connect();
			Assert::IsTrue(triggered);
		}

		TEST_METHOD(TestConnectAuthenticateRequested)
		{
			std::vector<signalr::value> parameter;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().Connect();
			Assert::AreEqual(size_t(1), parameter.size());
			Assert::IsTrue(parameter[0].is_map());
			Assert::IsTrue(parameter[0].as_map().contains("$type"));
			Assert::AreEqual(std::string("authenticate"), parameter[0].as_map().at("$type").as_string());
		}

		TEST_METHOD(TestConnectLogOnException)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::DEBUG;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { callback(std::move(std::make_exception_ptr("mock"))); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			CreateClient().Connect();
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::ERROR), static_cast<int>(logtype));
		}

		TEST_METHOD(TestConnectRefuseWhenConnected)
		{
			bool triggered = false;
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::DEBUG;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			auto client = CreateClient();
			client.Connect();
			triggered = false;
			logtype = Utility::Logging::LoggerInterface::LogLevel::DEBUG;

			client.Connect();
			Assert::IsFalse(triggered);
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::WARNING), static_cast<int>(logtype));
		}

		TEST_METHOD(TestDisconnectStopInvoked)
		{
			bool triggered = false;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Stop(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});
			auto client = CreateClient();
			client.Connect();

			client.Disconnect();
			Assert::IsTrue(triggered);
		}

		TEST_METHOD(TestDisconnectNothingBesidesStopInvoked)
		{
			bool triggered = false;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([&] (const std::string& method, const std::function<void(const std::vector<signalr::value>&)>& func) { triggered = true; });
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Stop(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([&] (const std::string&, const std::vector<signalr::value>&, std::function<void(const signalr::value&, std::exception_ptr)>) { triggered = true; });
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});
			auto client = CreateClient();
			client.Connect();
			triggered = false;

			client.Disconnect();
			Assert::IsFalse(triggered);
		}

		TEST_METHOD(TestLoadCharacterSendInvoked)
		{
			std::vector<signalr::value> parameter;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().LoadCharacter("somerandomguid");
			Assert::AreEqual(size_t(1), parameter.size());
			Assert::IsTrue(parameter[0].is_map());
			Assert::IsTrue(parameter[0].as_map().contains("$type"));
			Assert::AreEqual(std::string("loadCharacter"), parameter[0].as_map().at("$type").as_string());
		}

		TEST_METHOD(TestGetCharactersEmpty)
		{
			std::function<void(const std::vector<signalr::value>&)> parameter;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			Assert::AreEqual(size_t(0), client.GetCharacters().size());
			client.Connect();
			Assert::AreEqual(size_t(0), client.GetCharacters().size());
		}

		TEST_METHOD(TestGetCharacters)
		{
			std::function<void(const std::vector<signalr::value>&)> parameter;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			client.Connect();

			std::string id1 = "d9ef5777-3f04-e299-a7ac-3b9aad814da5", id2 = "67e139a4-e30e-4603-a083-6e89719a9bb1";
			std::string name1 = "Bella", name2 = "Catherine";
			std::string creatorNotes1 = "Magnetic woman", creatorNotes2 = "Cute and flirty";
			bool explicitContent1 = true, explicitContent2 = false;
			bool favorite1 = false, favorite2 = false;

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "charactersListLoaded" },
				{ "characters", std::vector<signalr::value> {
					std::map<std::string, signalr::value> {
						{ "id", id1 },
						{ "name", name1},
						{ "creatorNotes", creatorNotes1 },
						{ "explicitContent", explicitContent1 },
						{ "favorite", favorite1 } },
					std::map<std::string, signalr::value> {
						{ "id", id2 },
						{ "name", name2},
						{ "creatorNotes", creatorNotes2 },
						{ "explicitContent", explicitContent2 },
						{ "favorite", favorite2 } }
					}
				}})});

			auto& chars = client.GetCharacters();
			Assert::AreEqual(size_t(2), chars.size());
			Assert::AreEqual(id1, chars[0]->m_id);
			Assert::AreEqual(name1, chars[0]->m_name);
			Assert::AreEqual(creatorNotes1, chars[0]->m_creatorNotes);
			Assert::AreEqual(explicitContent1, chars[0]->m_explicitContent);
			Assert::AreEqual(favorite1, chars[0]->m_favorite);
			Assert::AreEqual(id2, chars[1]->m_id);
			Assert::AreEqual(name2, chars[1]->m_name);
			Assert::AreEqual(creatorNotes2, chars[1]->m_creatorNotes);
			Assert::AreEqual(explicitContent2, chars[1]->m_explicitContent);
			Assert::AreEqual(favorite2, chars[1]->m_favorite);
		}

		TEST_METHOD(TestGetUsernameEmpty)
		{
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			Assert::AreEqual("", client.GetUsername().data());
			client.Connect();
			Assert::AreEqual("", client.GetUsername().data());
		}

		TEST_METHOD(TestGetUsername)
		{
			std::function<void(const std::vector<signalr::value>&)> parameter;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			client.Connect();
			std::string userName("andsothenIsaid,that'snotanoven,that'smygrandma");

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "welcome" },
				{ "user", std::map<std::string, signalr::value> {
					{ "id", "433d835f-296b-47f9-8475-5a993760bc1a" },
					{ "name", userName }
				} }})});

			Assert::AreEqual(std::string_view(userName), client.GetUsername());
		}

		TEST_METHOD(TestGetChatSessionEmpty)
		{
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			Assert::IsNull(client.GetChatSession());
			client.Connect();
			Assert::IsNull(client.GetChatSession());
		}

		TEST_METHOD(TestGetChatSession)
		{
			std::function<void(const std::vector<signalr::value>&)> parameter;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			client.Connect();

			std::string id1 = "d9ef5777-3f04-e299-a7ac-3b9aad814da5";
			std::string name1 = "Bella";
			std::string creatorNotes1 = "Magnetic woman";
			bool explicitContent1 = true;
			bool favorite1 = false;
			std::string chatId = "a33a4c12-df9d-427a-8f3e-c54adcd261ca";
			std::string sessionId = "6fb7cfad-e38a-597f-e5c2-e63c6b0b7fc8";
			std::string messageId1 = "e9ec8058-fda9-4b69-b266-fd88bdfcdd14";
			std::string messageText1 = "Lmao I aint leaking my ai messages on github xd";
			std::string messageText2 = "Not cool bro, not cool...";
			std::string userId = "433d835f-296b-47f9-8475-5a993760bc1a";
			std::string userName = "ayowtf";

			std::string llmServiceId = "1a6d869c-42a3-8b58-29a7-50b57974f53e";
			std::string sttServiceId = "1a6d869c-42a3-8b58-29a7-50b57974f53e";
			std::string ttsServiceId = "1a6d869c-42a3-8b58-29a7-50b57974f53e";

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "welcome" },
				{ "user", std::map<std::string, signalr::value> {
					{ "id", userId },
					{ "name", userName }
				} }})});

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "charactersListLoaded" },
				{ "characters", std::vector<signalr::value> {
					std::map<std::string, signalr::value> {
						{ "id", id1 },
						{ "name", name1},
						{ "creatorNotes", creatorNotes1 },
						{ "explicitContent", explicitContent1 },
						{ "favorite", favorite1 } }
					}
				}})});

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "startChat" },
				{ "character", std::map<std::string, signalr::value> {
					{ "id", id1 } } },
				{ "characterId", id1 },
				{ "chatId", chatId }
			})});

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "chatStarted" },
				{ "chatId", chatId },
				{ "services", std::map<std::string, signalr::value> {
					{ "textGen", std::map<std::string, signalr::value> {
						{ "serviceName", "VoxtaCloud" },
						{ "serviceId", llmServiceId } } },
					{ "speechToText", std::map<std::string, signalr::value> {
						{ "serviceName", "VoxtaCloud" },
						{ "serviceId", sttServiceId } } },
					{ "textToSpeech", std::map<std::string, signalr::value> {
						{ "serviceName", "Coqui" },
						{ "serviceId", ttsServiceId } } }
				} },
				{ "characters", std::vector<signalr::value> {
					std::map<std::string, signalr::value> {
						{ "id", id1 },
						{ "name", name1 }
					}
					} },
				{ "sessionId", sessionId }
			})});

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "replyStart" },
				{ "messageId", messageId1 },
				{ "senderId", id1 },
				{ "sessionId", sessionId },
			})});

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "replyChunk" },
				{ "messageId", messageId1 },
				{ "senderId", id1 },
				{ "startIndex", 0 },
				{ "endIndex", 47 },
				{ "text", messageText1 },
				{ "audioUrl", "/api/tts/gens/etc..." },
				{ "sessionId", sessionId }
			})});

			parameter(std::vector<signalr::value> { signalr::value(std::map<std::string, signalr::value> {
				{ "$type", "replyEnd" },
				{ "messageId", messageId1 },
				{ "senderId", id1 },
				{ "sessionId", sessionId },
			})});

			auto chatSession = client.GetChatSession();

			Assert::AreEqual(chatId, chatSession->m_chatId);
			Assert::AreEqual(sessionId, chatSession->m_sessionId);

			Assert::AreEqual(size_t(1), chatSession->m_characters.size());
			Assert::AreSame(client.GetCharacters()[0].get(), chatSession->m_characters[0]);

			Assert::AreEqual(size_t(3), chatSession->m_services.size());

			Assert::AreEqual(ttsServiceId, chatSession->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::TEXT_TO_SPEECH).m_serviceId);
			Assert::AreEqual(sttServiceId, chatSession->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::SPEECH_TO_TEXT).m_serviceId);
			Assert::AreEqual(llmServiceId, chatSession->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::TEXT_GEN).m_serviceId);

			Assert::AreEqual(size_t(1), chatSession->m_chatMessages.size());
			auto msg = chatSession->m_chatMessages.begin();

			//std::string m_text = "";
			//std::vector<std::string> m_audioUrls;
			//const std::string m_messageId;
			//const std::string m_charId;
		}

		Voxta::VoxtaClient CreateClient()
		{
			signalRWrapper = std::move(mockWrapper);
			loggerWrapper = std::move(mockLogger);
			return Voxta::VoxtaClient(std::move(signalRWrapper), *loggerWrapper, stateChangeMock, requestingUserInputEventMock, charSpeakingEventMock);
		}
	};
}