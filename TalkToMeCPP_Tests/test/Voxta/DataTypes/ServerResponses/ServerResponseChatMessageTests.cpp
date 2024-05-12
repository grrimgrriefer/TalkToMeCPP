// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseChatMessage.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseChatMessageTests)
	{
	public:
		TEST_METHOD(TestServerResponseChatMessageGetType)
		{
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_START,
				"msg001", "user001", "session001");

			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::CHAT_MESSAGE == chatMessageResponse.GetType());
		}

		TEST_METHOD(TestServerResponseChatMessageMessageType)
		{
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType messageType =
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_CHUNK;

			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				messageType, "msg001", "user001", "session001");

			Assert::IsTrue(messageType == chatMessageResponse.m_messageType);
		}

		TEST_METHOD(TestServerResponseChatMessageMessageId)
		{
			std::string messageId = "msg001";
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_END,
				messageId, "user001", "session001");

			Assert::AreEqual(messageId, chatMessageResponse.m_messageId);
		}

		TEST_METHOD(TestServerResponseChatMessageSenderId)
		{
			std::string senderId = "user001";
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_START,
				"msg001", senderId, "session001");

			Assert::AreEqual(senderId, chatMessageResponse.m_senderId);
		}

		TEST_METHOD(TestServerResponseChatMessageSessionId)
		{
			std::string sessionId = "session001";
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_CHUNK,
				"msg001", "user001", sessionId);

			Assert::AreEqual(sessionId, chatMessageResponse.m_sessionId);
		}

		TEST_METHOD(TestServerResponseChatMessageStartIndex)
		{
			int startIndex = 0;
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_END,
				"msg001", "user001", "session001", startIndex, 10, "Hello, World!", "http://example.com/audio.mp3");

			Assert::AreEqual(startIndex, chatMessageResponse.m_startIndex);
		}

		TEST_METHOD(TestServerResponseChatMessageEndIndex)
		{
			int endIndex = 10;
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_START,
				"msg001", "user001", "session001", 0, endIndex, "Hello, World!", "http://example.com/audio.mp3");

			Assert::AreEqual(endIndex, chatMessageResponse.m_endIndex);
		}

		TEST_METHOD(TestServerResponseChatMessageText)
		{
			std::string messageText = "Hello, World!";
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_CHUNK,
				"msg001", "user001", "session001", 0, 10, messageText, "http://example.com/audio.mp3");

			Assert::AreEqual(messageText, chatMessageResponse.m_messageText);
		}

		TEST_METHOD(TestServerResponseChatMessageAudioUrlPath)
		{
			std::string audioUrlPath = "http://example.com/audio.mp3";
			Voxta::DataTypes::ServerResponses::ServerResponseChatMessage chatMessageResponse(
				Voxta::DataTypes::ServerResponses::ServerResponseChatMessage::MessageType::MESSAGE_END,
				"msg001", "user001", "session001", 0, 10, "Hello, World!", audioUrlPath);

			Assert::AreEqual(audioUrlPath, chatMessageResponse.m_audioUrlPath);
		}
	};
}