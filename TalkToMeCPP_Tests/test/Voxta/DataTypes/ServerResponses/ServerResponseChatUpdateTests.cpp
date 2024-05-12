// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseChatUpdate.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseChatUpdateTests)
	{
	public:
		TEST_METHOD(TestServerResponseChatUpdateGetType)
		{
			Voxta::DataTypes::ServerResponses::ServerResponseChatUpdate chatUpdateResponse("msg001", "user001", "Hello, World!", "session001");
			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::CHAT_UPDATE == chatUpdateResponse.GetType());
		}

		TEST_METHOD(TestServerResponseChatUpdateMessageId)
		{
			std::string messageId = "msg001";
			Voxta::DataTypes::ServerResponses::ServerResponseChatUpdate chatUpdateResponse(messageId, "user001", "Hello, World!", "session001");
			Assert::AreEqual(messageId, chatUpdateResponse.m_messageId);
		}

		TEST_METHOD(TestServerResponseChatUpdateSenderId)
		{
			std::string senderId = "user001";
			Voxta::DataTypes::ServerResponses::ServerResponseChatUpdate chatUpdateResponse("msg001", senderId, "Hello, World!", "session001");
			Assert::AreEqual(senderId, chatUpdateResponse.m_senderId);
		}

		TEST_METHOD(TestServerResponseChatUpdateText)
		{
			std::string text = "Hello, World!";
			Voxta::DataTypes::ServerResponses::ServerResponseChatUpdate chatUpdateResponse("msg001", "user001", text, "session001");
			Assert::AreEqual(text, chatUpdateResponse.m_text);
		}

		TEST_METHOD(TestServerResponseChatUpdateSessionId)
		{
			std::string sessionId = "session001";
			Voxta::DataTypes::ServerResponses::ServerResponseChatUpdate chatUpdateResponse("msg001", "user001", "Hello, World!", sessionId);
			Assert::AreEqual(sessionId, chatUpdateResponse.m_sessionId);
		}
	};
}