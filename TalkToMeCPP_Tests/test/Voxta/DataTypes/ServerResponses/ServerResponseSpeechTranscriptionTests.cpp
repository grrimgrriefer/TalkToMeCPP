// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseSpeechTranscription.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServerResponses/ServerResponseBase.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServerResponseSpeechTranscriptionTests)
	{
	public:
		TEST_METHOD(TestServerResponseSpeechTranscriptionGetType)
		{
			Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription speechTranscriptionResponse(
				"Just some random text", Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::END);

			Assert::IsTrue(Voxta::DataTypes::ServerResponses::ServerResponseType::SPEECH_TRANSCRIPTION == speechTranscriptionResponse.GetType());
		}

		TEST_METHOD(TestServerResponseSpeechTranscriptionTranscribedSpeech)
		{
			std::string transcribedSpeech = "Whatever the user said, idk?";

			Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription speechTranscriptionResponse(
				transcribedSpeech, Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::END);

			Assert::AreEqual(transcribedSpeech, speechTranscriptionResponse.m_transcribedSpeech);
		}

		TEST_METHOD(TestServerResponseSpeechTranscriptionTranscriptionState)
		{
			Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState transcriptionState =
				Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState::END;

			Voxta::DataTypes::ServerResponses::ServerResponseSpeechTranscription speechTranscriptionResponse(
				"it's responding time!", transcriptionState);

			Assert::IsTrue(transcriptionState == speechTranscriptionResponse.m_transcriptionState);
		}
	};
}