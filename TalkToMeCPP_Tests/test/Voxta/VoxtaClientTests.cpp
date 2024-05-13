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

		// Todo:
		// GetUsername()
		// GetCharacters();
		// GetChatSession()

		Voxta::VoxtaClient CreateClient()
		{
			signalRWrapper = std::move(mockWrapper);
			loggerWrapper = std::move(mockLogger);
			return Voxta::VoxtaClient(std::move(signalRWrapper), *loggerWrapper, stateChangeMock, requestingUserInputEventMock, charSpeakingEventMock);
		}
	};
}