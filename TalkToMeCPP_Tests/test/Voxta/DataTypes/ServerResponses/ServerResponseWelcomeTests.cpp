// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseWelcome.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseWelcomeTests)
	{
	public:
		TEST_METHOD(TestServerResponseWelcomeGetType)
		{
			Voxta::DataTypes::CharData userData("user001", "Alice");
			Voxta::DataTypes::ServerResponses::ServerResponseWelcome welcomeResponse(userData);

			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::WELCOME == welcomeResponse.GetType());
		}

		TEST_METHOD(TestServerResponseWelcomeUserData)
		{
			Voxta::DataTypes::CharData userData("user001", "Alice");
			Voxta::DataTypes::ServerResponses::ServerResponseWelcome welcomeResponse(userData);

			Assert::AreEqual(std::string("user001"), welcomeResponse.m_user.m_id);
			Assert::AreEqual(std::string("Alice"), welcomeResponse.m_user.m_name);
		}
	};
}