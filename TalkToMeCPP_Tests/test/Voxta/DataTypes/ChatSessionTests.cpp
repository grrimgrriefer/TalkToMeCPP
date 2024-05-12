// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ChatSession.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServiceData.h"
#include <map>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ChatSessionTests)
	{
	public:
		TEST_METHOD(TestChatSessionConstructorCharacters)
		{
			std::vector<const Voxta::DataTypes::CharData*> characters;
			characters.push_back(new Voxta::DataTypes::CharData("char001", "Alice"));
			characters.push_back(new Voxta::DataTypes::CharData("char002", "Bob"));

			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> services;
			Voxta::DataTypes::ChatSession chatSession(characters, "chat001", "session001", services);
			Assert::AreEqual(size_t(2), chatSession.m_characters.size());
			for (auto character : characters)
			{
				delete character;
			}
		}

		TEST_METHOD(TestChatSessionConstructorChatId)
		{
			std::vector<const Voxta::DataTypes::CharData*> characters;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> services;

			std::string chatId = "chat001";
			Voxta::DataTypes::ChatSession chatSession(characters, chatId, "session001", services);
			Assert::AreEqual(chatId, chatSession.m_chatId);
		}

		TEST_METHOD(TestChatSessionConstructorSessionId)
		{
			std::vector<const Voxta::DataTypes::CharData*> characters;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> services;

			std::string sessionId = "session001";
			Voxta::DataTypes::ChatSession chatSession(characters, "chat001", sessionId, services);
			Assert::AreEqual(sessionId, chatSession.m_sessionId);
		}

		TEST_METHOD(TestChatSessionConstructorServices)
		{
			std::vector<const Voxta::DataTypes::CharData*> characters;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> services;

			Voxta::DataTypes::ChatSession chatSession(characters, "chat001", "session001", services);
			Assert::AreEqual(size_t(0), chatSession.m_services.size());
		}

		TEST_METHOD(TestChatSessionChatMessagesEmptyByDefault)
		{
			std::vector<const Voxta::DataTypes::CharData*> characters;
			std::map<const Voxta::DataTypes::ServiceData::ServiceType, const Voxta::DataTypes::ServiceData> services;

			Voxta::DataTypes::ChatSession chatSession(characters, "chat001", "session001", services);
			Assert::IsTrue(chatSession.m_chatMessages.empty());
		}
	};
}