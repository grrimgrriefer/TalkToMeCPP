// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "MicrophoneWebSocket.h"
#include "AudioCaptureDevice.h"

namespace Utility::AudioInput
{
	class AudioInputWrapper
	{
	public:
		explicit AudioInputWrapper()
		{
			micWebSocket.OpenSocket();
		}
		~AudioInputWrapper()
		{
			micWebSocket.CloseSocket();
		}

		void StartStreaming()
		{
			if (!isStreaming)
			{
				audioDevice.startStream();
				micWebSocket.StartStreaming(audioDevice);
				isStreaming = true;
			}
		}

		void StopStreaming()
		{
			if (isStreaming)
			{
				audioDevice.stopStream();
				micWebSocket.StopStreaming();
				isStreaming = false;
			}
		}

		// Additional methods to interact with the streaming process can be added here

	private:
		MicrophoneWebSocket micWebSocket;
		AudioCaptureDevice audioDevice;
		bool isStreaming = false;

		// This method will be used by MicrophoneWebSocket to get audio data from AudioCaptureDevice
		static void OnAudioDataReceived(short* buffer, unsigned int nBufferFrames)
		{
			// Here you would add the logic to send the audio data through the WebSocket
		}
	};
}
