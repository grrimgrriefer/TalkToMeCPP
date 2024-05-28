// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioInputWrapper.h"
#include "AudioWebSocket.h"
#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace Utility::AudioInput
{
	AudioInputWrapper::AudioInputWrapper(std::string_view serverIP, int serverPort) :
		websocketPotato(std::make_shared<Utility::AudioInput::AudioWebSocket>(serverIP, serverPort))
	{
	}

	void AudioInputWrapper::StartStreaming()
	{
		if (isStartingUp || isStreaming)
		{
			return;
		}
		isStartingUp = true;

		std::jthread thread([this] ()
		{
			websocketPotato->connect();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			websocketPotato->send("{\"contentType\":\"audio/wav\",\"sampleRate\":16000,"
				"\"channels\":1,\"bitsPerSample\": 16,\"bufferMilliseconds\":30}");
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			audioDevice.RegisterSocket(websocketPotato);
			audioDevice.Initialize();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			audioDevice.startStream();
			isStreaming = true;
			isStartingUp = false;
		});
		thread.detach();
	}

	void AudioInputWrapper::StopStreaming()
	{
		if (isStartingUp || !isStreaming)
		{
			return;
		}

		audioDevice.stopStream();
		isStreaming = false;
	}
}