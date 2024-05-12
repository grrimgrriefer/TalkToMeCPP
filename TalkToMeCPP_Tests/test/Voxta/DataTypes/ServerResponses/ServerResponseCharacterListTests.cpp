// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseCharacterListTests)
	{
	public:
		TEST_METHOD(TestServerResponseCharacterListGetType)
		{
			std::vector<Voxta::DataTypes::CharData> characters = {
				Voxta::DataTypes::CharData("char001", "Alice"),
				Voxta::DataTypes::CharData("char002", "Bob")
			};
			Voxta::DataTypes::ServerResponses::ServerResponseCharacterList characterListResponse(characters);
			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::CHARACTER_LIST == characterListResponse.GetType());
		}

		TEST_METHOD(TestServerResponseCharacterListCharacters)
		{
			std::vector<Voxta::DataTypes::CharData> characters = {
				Voxta::DataTypes::CharData("char001", "Alice"),
				Voxta::DataTypes::CharData("char002", "Bob")
			};
			Voxta::DataTypes::ServerResponses::ServerResponseCharacterList characterListResponse(characters);
			Assert::AreEqual(size_t(2), characterListResponse.m_characters.size());
			Assert::AreEqual(std::string("char001"), characterListResponse.m_characters[0].m_id);
			Assert::AreEqual(std::string("Alice"), characterListResponse.m_characters[0].m_name);
			Assert::AreEqual(std::string("char002"), characterListResponse.m_characters[1].m_id);
			Assert::AreEqual(std::string("Bob"), characterListResponse.m_characters[1].m_name);
		}
	};
}