// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseCharacterLoadedTests)
	{
	public:
		TEST_METHOD(TestServerResponseCharacterLoadedGetType)
		{
			Voxta::DataTypes::ServerResponses::ServerResponseCharacterLoaded characterLoadedResponse("char001", true, std::vector<Voxta::DataTypes::ServerResponses::CharacterLoadedVoiceData>());
			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::CHARACTER_LOADED == characterLoadedResponse.GetType());
		}

		TEST_METHOD(TestServerResponseCharacterLoadedCharacterId)
		{
			std::string characterId = "char001";
			Voxta::DataTypes::ServerResponses::ServerResponseCharacterLoaded characterLoadedResponse(characterId, true, std::vector<Voxta::DataTypes::ServerResponses::CharacterLoadedVoiceData>());
			Assert::AreEqual(characterId, characterLoadedResponse.m_characterId);
		}

		TEST_METHOD(TestServerResponseCharacterLoadedEnableThinkingSpeech)
		{
			bool enableThinkingSpeech = true;
			Voxta::DataTypes::ServerResponses::ServerResponseCharacterLoaded characterLoadedResponse("char001", enableThinkingSpeech, std::vector<Voxta::DataTypes::ServerResponses::CharacterLoadedVoiceData>());
			Assert::IsTrue(enableThinkingSpeech == characterLoadedResponse.m_enableThinkingSpeech);
		}
	};
}