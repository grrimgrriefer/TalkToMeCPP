// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServiceData.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>
#include <map>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseChatStartedTests)
	{
	public:
		TEST_METHOD(TestServerResponseChatStartedGetType)
		{
			std::vector<std::string> characterIds = { "char001", "char002" };
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> serviceIds;

			Voxta::DataTypes::ServerResponses::ServerResponseChatStarted chatStartedResponse("user001", characterIds, serviceIds, "chat001", "session001");
			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::CHAT_STARTED == chatStartedResponse.GetType());
		}

		TEST_METHOD(TestServerResponseChatStartedUserId)
		{
			std::string userId = "user001";
			std::vector<std::string> characterIds;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> serviceIds;

			Voxta::DataTypes::ServerResponses::ServerResponseChatStarted chatStartedResponse(userId, characterIds, serviceIds, "chat001", "session001");
			Assert::AreEqual(userId, chatStartedResponse.m_userId);
		}

		TEST_METHOD(TestServerResponseChatStartedCharacterIds)
		{
			std::vector<std::string> characterIds = { "char001", "char002" };
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> serviceIds;

			Voxta::DataTypes::ServerResponses::ServerResponseChatStarted chatStartedResponse("user001", characterIds, serviceIds, "chat001", "session001");
			Assert::AreEqual(size_t(2), chatStartedResponse.m_characterIds.size());
			Assert::AreEqual(std::string("char001"), chatStartedResponse.m_characterIds[0]);
			Assert::AreEqual(std::string("char002"), chatStartedResponse.m_characterIds[1]);
		}

		TEST_METHOD(TestServerResponseChatStartedServiceIds)
		{
			std::vector<std::string> characterIds;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> serviceIds;

			Voxta::DataTypes::ServerResponses::ServerResponseChatStarted chatStartedResponse("user001", characterIds, serviceIds, "chat001", "session001");
			Assert::AreEqual(size_t(0), chatStartedResponse.m_serviceIds.size());
		}

		TEST_METHOD(TestServerResponseChatStartedChatId)
		{
			std::string chatId = "chat001";
			std::vector<std::string> characterIds;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> serviceIds;

			Voxta::DataTypes::ServerResponses::ServerResponseChatStarted chatStartedResponse("user001", characterIds, serviceIds, chatId, "session001");
			Assert::AreEqual(chatId, chatStartedResponse.m_chatId);
		}

		TEST_METHOD(TestServerResponseChatStartedSessionId)
		{
			std::string sessionId = "session001";
			std::vector<std::string> characterIds;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> serviceIds;

			Voxta::DataTypes::ServerResponses::ServerResponseChatStarted chatStartedResponse("user001", characterIds, serviceIds, "chat001", sessionId);
			Assert::AreEqual(sessionId, chatStartedResponse.m_sessionId);
		}
	};
}