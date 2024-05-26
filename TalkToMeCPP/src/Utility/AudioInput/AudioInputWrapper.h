// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "MicrophoneWebSocket.h"
#include "AudioCaptureDevice.h"
#include <memory>

namespace Utility::AudioInput
{
	class AudioInputWrapper
	{
	public:
		explicit AudioInputWrapper(std::string_view serverIP, int serverPort) :
			micWebSocket(std::make_shared<MicrophoneWebSocket>(serverIP, serverPort)),
			audioDevice(micWebSocket)
		{
		}

		void StartStreaming()
		{
			if (!isInitialized)
			{
				Initialize();
			}

			if (!isStreaming)
			{
				audioDevice.startStream();
				isStreaming = true;
			}
		}

	private:
		void Initialize() // Don't initialize in constructor, we wanna wait till Voxta client has authorized before opening socket
		{
			micWebSocket->Start();
			audioDevice.Initialize();
			isInitialized = true;
		}

		std::shared_ptr<MicrophoneWebSocket> micWebSocket;
		AudioCaptureDevice audioDevice;
		bool isStreaming = false;
		bool isInitialized = false;
	};
}
