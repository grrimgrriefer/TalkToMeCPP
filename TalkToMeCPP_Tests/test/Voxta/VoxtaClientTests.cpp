// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaClient.cpp"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiHandler.h"
#include "../TalkToMeCPP/src/Voxta/VoxtaApiHandler.cpp"
#include "../TalkToMeCPP/src/Logging/HubConnectionLogger.h"
#include "../TalkToMeCPP/src/Logging/HubConnectionLogger.cpp"
#include "../TalkToMeCPP/src/Logging/ThreadedLogger.h"
#include "../TalkToMeCPP/src/Logging/ThreadedLogger.cpp"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ChatMessage.h"
#include "../TalkToMeCPP/src/Voxta/SignalRWrapperInterface.h"
#include "../TalkToMeCPP/src/Voxta/SignalRWrapper.h"
#include <functional>
#include <string>
#include "gmock/gmock.h"
#include <gmock/gmock-function-mocker.h>
#include <gmock/gmock-spec-builders.h>
#include <gmock/gmock-actions.h>
#include <signalrclient/connection_state.h>
#include <memory>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VoxtaClientTests
{
	class MockHubConnection : public Voxta::SignalRWrapperInterface
	{
	public:
		MOCK_METHOD(void, start, (std::function<void(std::exception_ptr)>), (noexcept, override));
		MOCK_METHOD(void, stop, (std::function<void(std::exception_ptr)>), (noexcept, override));
		MOCK_METHOD(void, on, (const std::string&, const std::function<void(const std::vector<signalr::value>&)>&), (override));
		MOCK_METHOD(void, invoke, (const std::string&, const std::vector<signalr::value>&, std::function<void(const signalr::value&, std::exception_ptr)>), (noexcept, override));

		// Constructor mock (if needed)
		MockHubConnection()
		{}
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

		std::unique_ptr<Voxta::SignalRWrapperInterface> signalRWrapper;
		Logging::ThreadedLogger loggerMock = Logging::ThreadedLogger("testlogfile.txt");

		TEST_METHOD_INITIALIZE(MethodInitialize)
		{
			newState = Voxta::VoxtaClient::VoxtaClientState::DISCONNECTED;
			userInput = "";
			lastChatMessage = nullptr;
			lastCharData = nullptr;

			auto wrapper = std::make_unique<MockHubConnection>();
			std::unique_ptr<Voxta::SignalRWrapperInterface> interface = std::move(wrapper);
			signalRWrapper = std::move(interface);
		}

		TEST_METHOD(TestConstructorDisconnected)
		{
			Voxta::VoxtaClient client(std::move(signalRWrapper), loggerMock, stateChangeMock, requestingUserInputEventMock, charSpeakingEventMock);
			Assert::IsTrue(newState == Voxta::VoxtaClient::VoxtaClientState::DISCONNECTED);
		}

		TEST_METHOD(TestListeningOnConnect)
		{
			std::string methodName;
			std::function<void(const std::vector<signalr::value>&)> function;
			auto wrapper = std::make_unique<MockHubConnection>();

			auto raw = wrapper.get();
			EXPECT_CALL(*raw, on(testing::_, testing::_)).WillOnce(testing::SaveArg<0>(&methodName));
			EXPECT_CALL(*raw, start(testing::_)).WillOnce([] (std::function<void(std::exception_ptr)> callback) { callback(nullptr); });
			EXPECT_CALL(*raw, invoke(testing::_, testing::_, testing::_));

			std::unique_ptr<Voxta::SignalRWrapperInterface> interface = std::move(wrapper);
			signalRWrapper = std::move(interface);

			Voxta::VoxtaClient client(std::move(signalRWrapper), loggerMock, stateChangeMock, requestingUserInputEventMock, charSpeakingEventMock);
			client.Connect();
			Assert::IsTrue(methodName == "ReceiveMessage");
		}
	};
}