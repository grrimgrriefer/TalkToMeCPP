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
		explicit AudioInputWrapper() : micWebSocket(std::make_shared<MicrophoneWebSocket>()),
			audioDevice(micWebSocket)
		{
		}

		~AudioInputWrapper()
		{
			micWebSocket->CloseSocket();
		}

		void Initialize()
		{
			micWebSocket->OpenSocket();
			audioDevice.Initialize();
		}

		void StartStreaming()
		{
			if (!isStreaming)
			{
				audioDevice.startStream(&MicrophoneWebSocket::SendData);
				isStreaming = true;
			}
		}

		/*	void StopStreaming()
			{
				if (isStreaming)
				{
					audioDevice.stopStream();
					isStreaming = false;
				}
			}
		*/

		// Additional methods to interact with the streaming process can be added here

	private:
		std::shared_ptr<MicrophoneWebSocket> micWebSocket;
		AudioCaptureDevice audioDevice;
		bool isStreaming = false;
	};
}
