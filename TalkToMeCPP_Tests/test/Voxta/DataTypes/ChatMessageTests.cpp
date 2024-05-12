// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ChatMessage.h"
#include <memory>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ChatMessageTests)
	{
	public:
		TEST_METHOD(TestChatMessageConstructorWithTextMessageId)
		{
			std::string messageId = "msg001";
			std::string charId = "char001";
			std::string text = "Hello, World!";

			Voxta::DataTypes::ChatMessage chatMessage(messageId, charId, text);
			Assert::AreEqual(messageId, chatMessage.m_messageId);
		}

		TEST_METHOD(TestChatMessageConstructorWithTextCharId)
		{
			std::string messageId = "msg001";
			std::string charId = "char001";
			std::string text = "Hello, World!";

			Voxta::DataTypes::ChatMessage chatMessage(messageId, charId, text);
			Assert::AreEqual(charId, chatMessage.m_charId);
		}

		TEST_METHOD(TestChatMessageConstructorWithTextText)
		{
			std::string messageId = "msg001";
			std::string charId = "char001";
			std::string text = "Hello, World!";

			Voxta::DataTypes::ChatMessage chatMessage(messageId, charId, text);
			Assert::AreEqual(text, chatMessage.m_text);
		}

		TEST_METHOD(TestChatMessageConstructorWithoutTextMessageId)
		{
			std::string messageId = "msg002";
			std::string charId = "char002";

			Voxta::DataTypes::ChatMessage chatMessage(messageId, charId);
			Assert::AreEqual(messageId, chatMessage.m_messageId);
		}

		TEST_METHOD(TestChatMessageConstructorWithoutTextCharId)
		{
			std::string messageId = "msg002";
			std::string charId = "char002";

			Voxta::DataTypes::ChatMessage chatMessage(messageId, charId);
			Assert::AreEqual(charId, chatMessage.m_charId);
		}

		TEST_METHOD(TestChatMessageConstructorWithoutTextText)
		{
			std::string messageId = "msg002";
			std::string charId = "char002";

			Voxta::DataTypes::ChatMessage chatMessage(messageId, charId);
			Assert::AreEqual(std::string(""), chatMessage.m_text);
		}

		TEST_METHOD(TestChatMessageDefaultAudioUrls)
		{
			Voxta::DataTypes::ChatMessage chatMessage("msg003", "char003");
			Assert::IsTrue(chatMessage.m_audioUrls.empty());
		}

		TEST_METHOD(TestChatMessageAddAudioUrl)
		{
			Voxta::DataTypes::ChatMessage chatMessage("msg004", "char004");
			chatMessage.m_audioUrls.emplace_back("https://example.com/audio001.wav");
			Assert::AreEqual(size_t(1), chatMessage.m_audioUrls.size());
			Assert::AreEqual(std::string("https://example.com/audio001.wav"), chatMessage.m_audioUrls[0]);
		}

		TEST_METHOD(TestChatMessageIdComparerTrue)
		{
			auto chatMessage = std::make_unique<Voxta::DataTypes::ChatMessage>("msg005", "char005");
			Voxta::DataTypes::ChatMessageIdComparer comparer("msg005");
			Assert::IsTrue(comparer(chatMessage));
		}

		TEST_METHOD(TestChatMessageIdComparerFalse)
		{
			auto chatMessage = std::make_unique<Voxta::DataTypes::ChatMessage>("msg006", "char006");
			Voxta::DataTypes::ChatMessageIdComparer comparer("msg007");
			Assert::IsFalse(comparer(chatMessage));
		}
	};
}