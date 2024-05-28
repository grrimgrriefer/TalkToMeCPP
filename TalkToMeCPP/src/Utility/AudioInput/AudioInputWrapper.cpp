// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioInputWrapper.h"
#include "AudioWebSocket.h"

namespace Utility::AudioInput
{
	AudioInputWrapper::AudioInputWrapper(std::string_view serverIP, int serverPort) :
		websocketPotato(std::make_shared<Utility::AudioInput::AudioWebSocket>(serverIP, serverPort))
	{
	}

	void AudioInputWrapper::OpenAudioSocket()
	{
		websocketPotato->connect();
	}

	void AudioInputWrapper::ConnectMicrophoneSocket()
	{
		websocketPotato->send("{\"contentType\":\"audio/wav\",\"sampleRate\":16000,"
			"\"channels\":1,\"bitsPerSample\": 16,\"bufferMilliseconds\":30}");
	}

	void AudioInputWrapper::RegisterPotato()
	{
		audioDevice.RegisterSocket(websocketPotato);
		audioDevice.Initialize();
		isInitialized = true;
	}

	void AudioInputWrapper::StartStreaming()
	{
		if (!isStreaming)
		{
			audioDevice.startStream();
			isStreaming = true;
		}
	}

	void AudioInputWrapper::StopStreaming()
	{
		if (!isStreaming)
		{
			audioDevice.stopStream();
			isStreaming = false;
		}
	}
}