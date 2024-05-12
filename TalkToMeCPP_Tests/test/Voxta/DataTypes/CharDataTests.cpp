// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/CharData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(CharDataTests)
	{
	public:
		TEST_METHOD(TestCharDataConstructorId)
		{
			std::string id = "testID";
			Voxta::DataTypes::CharData character(id, "sillybilly");
			Assert::AreEqual(id, character.m_id);
		}

		TEST_METHOD(TestCharDataConstructorName)
		{
			std::string name = "sillybilly";
			Voxta::DataTypes::CharData character("testID", name);
			Assert::AreEqual(name, character.m_name);
		}

		TEST_METHOD(TestCharDataConstructorDefaultCreatorNotes)
		{
			Voxta::DataTypes::CharData character("", "");
			Assert::AreEqual(std::string(""), character.m_creatorNotes);
		}

		TEST_METHOD(TestCharDataConstructorDefaultExplicitContent)
		{
			Voxta::DataTypes::CharData character("", "");
			Assert::IsFalse(character.m_explicitContent);
		}

		TEST_METHOD(TestCharDataConstructorDefaultFavorite)
		{
			Voxta::DataTypes::CharData character("", "");
			Assert::IsFalse(character.m_favorite);
		}

		TEST_METHOD(TestCharDataIdComparerTrue)
		{
			auto character = std::make_unique<Voxta::DataTypes::CharData>("testID", "");
			Voxta::DataTypes::CharDataIdComparer comparer("testID");
			Assert::IsTrue(comparer(character));
		}

		TEST_METHOD(TestCharDataIdComparerFalse)
		{
			auto character = std::make_unique<Voxta::DataTypes::CharData>("testID", "");
			Voxta::DataTypes::CharDataIdComparer comparer("wrongID");
			Assert::IsFalse(comparer(character));
		}
	};
}