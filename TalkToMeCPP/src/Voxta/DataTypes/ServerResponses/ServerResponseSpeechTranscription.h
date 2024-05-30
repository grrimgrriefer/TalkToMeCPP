// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <string>

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for Welcome response from the server.
	/// Should be pretty obvious
	/// </summary>
	struct ServerResponseSpeechTranscription : ServerResponseBase
	{
		enum class TranscriptionState
		{
			PARTIAL,
			END,
			CANCELLED
		};

		const TranscriptionState m_transcriptionState;
		const std::string m_transcribedSpeech;

		ServerResponseType GetType() final
		{
			return ServerResponseType::SPEECH_TRANSCRIPTION;
		}

		explicit ServerResponseSpeechTranscription(std::string_view transcribedSpeech,
				TranscriptionState transcriptionState) :
			m_transcriptionState(transcriptionState),
			m_transcribedSpeech(transcribedSpeech)
		{
		}
	};
}