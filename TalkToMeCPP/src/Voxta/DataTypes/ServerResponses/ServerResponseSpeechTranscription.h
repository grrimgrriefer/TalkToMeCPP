// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for Welcome response from the server.
	/// Should be pretty obvious
	/// </summary>
	struct ServerResponseSpeechTranscription : ServerResponseBase
	{
		ServerResponseType GetType() final
		{
			return ServerResponseType::SPEECH_TRANSCRIPTION;
		}

		enum class TranscriptionState
		{
			PARTIAL, END, CANCELLED
		};

		explicit ServerResponseSpeechTranscription(std::string_view transcribedSpeech, TranscriptionState transcriptionState) :
			m_transcriptionState(transcriptionState), m_transcribedSpeech(transcribedSpeech)
		{
		}

		const TranscriptionState m_transcriptionState;
		const std::string m_transcribedSpeech;
	};
}