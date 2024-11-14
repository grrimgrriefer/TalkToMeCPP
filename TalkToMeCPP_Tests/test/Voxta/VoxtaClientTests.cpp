// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "pch.h"
#include "gmock/gmock.h"
#include "CppUnitTest.h"
#include "../MockProviders.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiRequestHandler.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiRequestHandler.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiResponseHandler.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiResponseHandler.cpp"
#include "../TalkToMeCPP/src/Utility/Logging/LoggerInterface.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ChatMessage.h"
#include "../TalkToMeCPP/src/Utility/SignalR/SignalRWrapperInterface.h"
#include "../TalkToMeCPP/src/Utility/GuidUtility.h"
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

namespace TalkToMeCPPTests
{
	TEST_CLASS(VoxtaClientTests)
	{
	public:
		// Mocks for the callback functions
		Voxta::VoxtaClient::VoxtaClientState newState;
		std::string userInput;
		std::string sttTranscribedText;
		bool sttIsFinalized;
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
		std::function<void(std::string_view, bool)> speechTranscribingEventMock = [this] (std::string_view transcribedText, bool isFinalized)
			{
				sttTranscribedText = transcribedText;
				sttIsFinalized = isFinalized;
			};
		std::function<void(std::string_view)> fatalErrorMock = [this] (std::string_view transcribedText)
			{
				sttTranscribedText = transcribedText;
			};
		std::function<void(const Voxta::DataTypes::ChatMessage*, const Voxta::DataTypes::CharData*)> charSpeakingEventMock = [this] (const Voxta::DataTypes::ChatMessage* chatMsg, const Voxta::DataTypes::CharData* charData)
			{
				lastChatMessage = chatMsg;
				lastCharData = charData;
			};

		// Mocks for SignalR and ThreadedLogger
		std::unique_ptr<MockProviders::MockHubConnection> mockWrapper;
		std::unique_ptr<MockProviders::MockLogger> mockLogger;
		std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> signalRWrapper;
		std::unique_ptr<Utility::Logging::LoggerInterface> loggerWrapper;

		TEST_METHOD_INITIALIZE(MethodInitialize)
		{
			newState = Voxta::VoxtaClient::VoxtaClientState::DISCONNECTED;
			userInput = "";
			sttTranscribedText = "";
			sttIsFinalized = false;
			lastChatMessage = nullptr;
			lastCharData = nullptr;

			mockWrapper = std::make_unique<MockProviders::MockHubConnection>();
			mockLogger = std::make_unique<MockProviders::MockLogger>();
		}

		TEST_METHOD(TestConnectStartListening)
		{
			std::string methodName;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&methodName));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().Connect();
			Assert::AreEqual(std::string("ReceiveMessage"), methodName);
		}

		TEST_METHOD(TestConnectStartInvoked)
		{
			bool triggered = false;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().Connect();
			Assert::IsTrue(triggered);
		}

		TEST_METHOD(TestConnectReceiveInvalidFormats)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;
			std::function<void(const std::vector<signalr::value>&)> invokerCallback;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&invokerCallback));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			CreateClient().Connect();

			invokerCallback(std::vector<signalr::value>{ }); // null
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
			logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;

			invokerCallback(std::vector<signalr::value>{ std::vector<uint8_t>{ 0 } }); //bytes
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
			logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;

			invokerCallback(std::vector<signalr::value>{ false }); // bool
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
			logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;

			invokerCallback(std::vector<signalr::value>{ "heyo" }); // string
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
			logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;

			invokerCallback(std::vector<signalr::value>{ std::vector<signalr::value> { true, false } }); // array
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
		}

		TEST_METHOD(TestConnectAuthenticateRequested)
		{
			std::vector<signalr::value> parameter;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClient().Connect();
			Assert::AreEqual(size_t(1), parameter.size());
			Assert::IsTrue(parameter[0].is_map());
			Assert::IsTrue(parameter[0].as_map().contains("$type"));
			Assert::AreEqual(std::string("authenticate"), parameter[0].as_map().at("$type").as_string());
		}

		TEST_METHOD(TestConnectAuthenticateRequestResponseHandled)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;

			std::function<void(const signalr::value&, std::exception_ptr)> parameter;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<2>(&parameter));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			CreateClient().Connect();
			parameter(MockProviders::GetWelcomeResponse(Utility::GuidUtility::GenerateGuid(), "some random username idk, idc"), nullptr);

			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Info), static_cast<int>(logtype));
		}

		TEST_METHOD(TestConnectLogOnException)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(std::move(std::make_exception_ptr("mock"))); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			CreateClient().Connect();
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
		}

		TEST_METHOD(TestConnectRefuseWhenConnected)
		{
			bool triggered = false;
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			auto client = CreateClient();
			client.Connect();
			triggered = false;
			logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;

			client.Connect();
			Assert::IsFalse(triggered);
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Warning), static_cast<int>(logtype));
		}

		TEST_METHOD(TestDisconnectWhenAlreadyDisconnectedLogWarning)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Stop(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));
			auto client = CreateClient();
			client.Connect();

			client.Disconnect();
			client.Disconnect();

			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Warning), static_cast<int>(logtype));
		}

		TEST_METHOD(TestDisconnectStopInvoked)
		{
			bool triggered = false;
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Stop(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { triggered = true; callback(nullptr); });
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
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { triggered = true; callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Stop(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([&] (const std::string&, const std::vector<signalr::value>&, const std::function<void(const signalr::value&, std::exception_ptr)>&) { triggered = true; });
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});
			auto client = CreateClient();
			client.Connect();
			triggered = false;

			client.Disconnect();
			Assert::IsFalse(triggered);
		}

		TEST_METHOD(TestForceStopImmediate)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;
			std::function<void(const std::vector<signalr::value>&)> capturedFunc;

			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([&] (const std::string& method, const std::function<void(const std::vector<signalr::value>&)>& func) { capturedFunc = func; });
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Stop(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));
			auto client = CreateClient();
			client.Connect();

			client.ForceStopImmediate();
			capturedFunc(std::vector<signalr::value> {MockProviders::GetCharactersListLoadedResponse(
				{ Utility::GuidUtility::GenerateGuid() }, { "Bella" }, { "Magnetic woman" }, { true }, { false })});

			auto characters = client.GetCharacters();
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
			Assert::AreEqual(size_t(0), characters.size());
		}

		TEST_METHOD(TestErrorForMissingService)
		{
			Utility::Logging::LoggerInterface::LogLevel logtype = Utility::Logging::LoggerInterface::LogLevel::Debug;
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;

			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault(testing::SaveArg<0>(&logtype));

			CreateClientWithChatInitialized(receiveMessageMockFunc, false, false, false);
			Assert::AreEqual(static_cast<int>(Utility::Logging::LoggerInterface::LogLevel::Error), static_cast<int>(logtype));
		}

		TEST_METHOD(TestNotifyAudioPlaybackStartSendInvoked)
		{
			std::vector<signalr::value> param;
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&param));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClientWithChatInitialized(receiveMessageMockFunc).NotifyAudioPlaybackStart(Utility::GuidUtility::GenerateGuid(), 0, 1, 0.5);
			Assert::AreEqual(size_t(1), param.size());
			Assert::IsTrue(param[0].is_map());
			Assert::IsTrue(param[0].as_map().contains("$type"));
			Assert::AreEqual(std::string("speechPlaybackStart"), param[0].as_map().at("$type").as_string());
		}

		TEST_METHOD(TestNotifyAudioPlaybackCompleteSendInvoked)
		{
			std::vector<signalr::value> parameter;
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			CreateClientWithChatInitialized(receiveMessageMockFunc).NotifyAudioPlaybackComplete(Utility::GuidUtility::GenerateGuid());
			Assert::AreEqual(size_t(1), parameter.size());
			Assert::IsTrue(parameter[0].is_map());
			Assert::IsTrue(parameter[0].as_map().contains("$type"));
			Assert::AreEqual(std::string("speechPlaybackComplete"), parameter[0].as_map().at("$type").as_string());
		}

		TEST_METHOD(TestGetCharactersEmpty)
		{
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			Assert::AreEqual(size_t(0), client.GetCharacters().size());
			client.Connect();
			Assert::AreEqual(size_t(0), client.GetCharacters().size());
		}

		TEST_METHOD(TestGetCharacters)
		{
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			client.Connect();

			std::string id1 = Utility::GuidUtility::GenerateGuid(), id2 = Utility::GuidUtility::GenerateGuid();
			std::string name1 = "Bella", name2 = "Catherine";
			std::string creatorNotes1 = "Magnetic woman", creatorNotes2 = "Cute and flirty";
			bool explicitContent1 = true, explicitContent2 = false;
			bool favorite1 = false, favorite2 = false;

			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetCharactersListLoadedResponse({ id1 , id2 }, { name1 , name2 },
				{ creatorNotes1 , creatorNotes2 }, { explicitContent1 , explicitContent2 }, { favorite1 , favorite2 })});

			auto chars = client.GetCharacters();
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
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
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
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			client.Connect();
			std::string userName("andsothenIsaid,that'snotanoven,that'smygrandma");

			parameter(std::vector<signalr::value> { MockProviders::GetWelcomeResponse(Utility::GuidUtility::GenerateGuid(), userName) });

			Assert::AreEqual(std::string_view(userName), client.GetUsername());
		}

		TEST_METHOD(TestGetChatSessionEmpty)
		{
			ON_CALL(*mockWrapper.get(), On(testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			auto client = CreateClient();
			Assert::IsNull(client.GetChatSession());
			client.Connect();
			Assert::IsNull(client.GetChatSession());
		}

		TEST_METHOD(TestGetChatReplyReponseHandling)
		{
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke(testing::_, testing::_, testing::_)).WillByDefault([] () {});
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string charName = "Bella";
			std::string creatorNotes1 = "Magnetic woman";
			bool explicitContent1 = true;
			bool favorite1 = false;
			std::string chatId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string messageId1 = Utility::GuidUtility::GenerateGuid();
			std::string messageText1 = "Lmao I aint leaking my ai messages on github xd";
			std::string audioUrl1 = "/api/tts/gens/etc...";
			std::string userId = Utility::GuidUtility::GenerateGuid();
			std::string userName = "ayowtf";
			std::string llmServiceId = Utility::GuidUtility::GenerateGuid();
			std::string sttServiceId = Utility::GuidUtility::GenerateGuid();
			std::string ttsServiceId = Utility::GuidUtility::GenerateGuid();

			auto client = CreateClient();
			client.Connect();

			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetWelcomeResponse(userId, userName) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetCharactersListLoadedResponse({ charId }, { charName },
				{ creatorNotes1 }, { explicitContent1 }, { favorite1 })});
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetStartChatResponse(charId, chatId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetChatStartedResponse(chatId, userId, userName,
				llmServiceId, sttServiceId, ttsServiceId, charId, charName, sessionId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetReplyStartResponse(messageId1, charId, sessionId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetReplyChunkResponse(messageId1, charId, static_cast<double>(0),
				static_cast<double>(47), messageText1, audioUrl1, sessionId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetReplyEndResponse(messageId1, charId, sessionId) });

			auto chatSession = client.GetChatSession();

			Assert::AreEqual(chatId, chatSession->m_chatId);
			Assert::AreEqual(sessionId, chatSession->m_sessionId);

			Assert::AreEqual(size_t(1), chatSession->m_characters.size());
			Assert::IsTrue(client.GetCharacters()[0] == chatSession->m_characters[0]); // force compare pointers here, not using AreSame on purpose

			Assert::AreEqual(size_t(3), chatSession->m_services.size());

			Assert::AreEqual(ttsServiceId, chatSession->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::TEXT_TO_SPEECH).m_serviceId);
			Assert::AreEqual(sttServiceId, chatSession->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::SPEECH_TO_TEXT).m_serviceId);
			Assert::AreEqual(llmServiceId, chatSession->m_services.at(Voxta::DataTypes::ServiceData::ServiceType::TEXT_GEN).m_serviceId);

			Assert::AreEqual(size_t(1), chatSession->m_chatMessages.size());
			auto msg = chatSession->m_chatMessages.begin();
			Assert::AreEqual(messageText1, msg->get()->m_text);
			Assert::AreEqual(size_t(1), msg->get()->m_audioUrls.size());
			Assert::AreEqual(audioUrl1, msg->get()->m_audioUrls[0]);
			Assert::AreEqual(messageId1, msg->get()->m_messageId);
			Assert::AreEqual(charId, msg->get()->m_charId);
		}

		TEST_METHOD(TestGetChatWrittenUserInput)
		{
			std::vector<signalr::value> serverRequestMessage;
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;

			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&serverRequestMessage));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			std::string charId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = Utility::GuidUtility::GenerateGuid();
			std::string messageId1 = Utility::GuidUtility::GenerateGuid();
			std::string messageId2 = Utility::GuidUtility::GenerateGuid();
			std::string userId = Utility::GuidUtility::GenerateGuid();

			userInput = "This is just something that the use would have inputted.";

			auto client = CreateClientWithChatInitialized(receiveMessageMockFunc, true, true, true, false, charId, sessionId, userId);
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetReplyStartResponse(messageId1, charId, sessionId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetReplyChunkResponse(messageId1, charId, static_cast<double>(0),
				static_cast<double>(47), "Char text example", "/api/tts/gens/etc...", sessionId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetReplyEndResponse(messageId1, charId, sessionId) });

			Assert::AreEqual(size_t(1), serverRequestMessage.size());
			Assert::IsTrue(serverRequestMessage[0].is_map());
			Assert::IsTrue(serverRequestMessage[0].as_map().contains("$type"));
			Assert::AreEqual(std::string("send"), serverRequestMessage[0].as_map().at("$type").as_string());

			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetUpdateChatResponse(messageId2, userId, userInput, sessionId) });

			auto chatSession = client.GetChatSession();
			Assert::AreEqual(size_t(2), chatSession->m_chatMessages.size());

			for (auto& chatMessage : chatSession->m_chatMessages)
			{
				// code block to be executed
				if (chatMessage->m_messageId == messageId2)
				{
					Assert::AreEqual(userInput, chatMessage->m_text);
					Assert::AreEqual(size_t(0), chatMessage->m_audioUrls.size());
					Assert::AreEqual(userId, chatMessage->m_charId);
					break;
				}
			}
		}

		TEST_METHOD(TestSpeechTranscriptionHandling)
		{
			std::vector<signalr::value> parameter;
			std::function<void(const std::vector<signalr::value>&)> receiveMessageMockFunc;
			ON_CALL(*mockWrapper.get(), On("ReceiveMessage", testing::_)).WillByDefault(testing::SaveArg<1>(&receiveMessageMockFunc));
			ON_CALL(*mockWrapper.get(), Start(testing::_)).WillByDefault([&] (const std::function<void(std::exception_ptr)>& callback) { callback(nullptr); });
			ON_CALL(*mockWrapper.get(), Invoke("SendMessage", testing::_, testing::_)).WillByDefault(testing::SaveArg<1>(&parameter));
			ON_CALL(*mockLogger.get(), LogMessage(testing::_, testing::_)).WillByDefault([] () {});

			std::string speechTranscription = "This is just something that the use would have inputted.";
			auto client = CreateClientWithChatInitialized(receiveMessageMockFunc);

			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetSpeechRecognitionEndResponse(speechTranscription, true) });
			Assert::IsTrue(sttTranscribedText.empty());
			Assert::IsFalse(sttIsFinalized);

			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetSpeechRecognitionEndResponse(speechTranscription, false) });
			Assert::AreEqual(speechTranscription, sttTranscribedText);
			Assert::IsTrue(sttIsFinalized);
		}

		Voxta::VoxtaClient CreateClient(bool useMicrophoneInput = true)
		{
			signalRWrapper = std::move(mockWrapper);
			loggerWrapper = std::move(mockLogger);
			return Voxta::VoxtaClient(std::move(signalRWrapper), *loggerWrapper, useMicrophoneInput, stateChangeMock, requestingUserInputEventMock, speechTranscribingEventMock, fatalErrorMock, charSpeakingEventMock);
		}

		Voxta::VoxtaClient CreateClientWithChatInitialized(const std::function<void(const std::vector<signalr::value>&)>& receiveMessageMockFunc,
			bool includdLLM = true,
			bool includeSTT = true,
			bool includeTTS = true,
			bool useMicrophoneInput = true,
			std::string_view charIdParam = "",
			std::string_view sessionIdParam = "",
			std::string_view userIdParam = "")
		{
			std::string charId = charIdParam.empty() ? Utility::GuidUtility::GenerateGuid() : charIdParam.data();
			std::string charName = "Bella";
			std::string creatorNotes1 = "Magnetic woman";
			bool explicitContent1 = true;
			bool favorite1 = false;
			std::string chatId = Utility::GuidUtility::GenerateGuid();
			std::string sessionId = sessionIdParam.empty() ? Utility::GuidUtility::GenerateGuid() : sessionIdParam.data();
			std::string messageId1 = Utility::GuidUtility::GenerateGuid();
			std::string messageText1 = "Lmao I aint leaking my ai messages on github xd";
			std::string audioUrl1 = "/api/tts/gens/etc...";
			std::string userId = userIdParam.empty() ? Utility::GuidUtility::GenerateGuid() : userIdParam.data();
			std::string userName = "ayowtf";
			std::string llmServiceId = includdLLM ? Utility::GuidUtility::GenerateGuid() : "";
			std::string sttServiceId = includeSTT ? Utility::GuidUtility::GenerateGuid() : "";
			std::string ttsServiceId = includeTTS ? Utility::GuidUtility::GenerateGuid() : "";

			auto client = CreateClient(useMicrophoneInput);
			client.Connect();

			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetWelcomeResponse(userId, userName) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetCharactersListLoadedResponse({ charId }, { charName },
				{ creatorNotes1 }, { explicitContent1 }, { favorite1 })});
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetStartChatResponse(charId, chatId) });
			receiveMessageMockFunc(std::vector<signalr::value> { MockProviders::GetChatStartedResponse(chatId, userId, userName,
				llmServiceId, sttServiceId, ttsServiceId, charId, charName, sessionId) });

			return client;
		}
	};
}