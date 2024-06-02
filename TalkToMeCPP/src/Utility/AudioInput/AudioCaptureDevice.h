// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include "AudioWebSocket.h"
#include <memory>
#include <mutex>
#include <rtaudio/RtAudio.h>
#include <string>

namespace Utility::AudioInput
{
	/// <summary>
	/// Class that handles the audio input from the microphone and converts it into
	/// char* buffers which are sent into the AudioWebSocket
	/// </summary>
	class AudioCaptureDevice
	{
	public:
		explicit AudioCaptureDevice(Logging::LoggerInterface& logger);

		void RegisterSocket(std::shared_ptr<Utility::AudioInput::AudioWebSocket> socket);

		bool IsInitialized();
		bool TryInitialize();

		void StartStream();
		void StopStream();
		void ReceiveAudioInputData(const char* buffer, unsigned int nBufferFrames);

		std::string_view GetDeviceName();

	private:
		std::mutex m_mutex;
		Logging::LoggerInterface& m_logger;

		std::string m_deviceName;
		std::unique_ptr<RtAudio> microphoneApi = std::make_unique<RtAudio>();
		std::shared_ptr<Utility::AudioInput::AudioWebSocket> m_socket;

		static int AudioCallback(void* outputBuffer,
			void* inputBuffer,
			unsigned int nBufferFrames,
			double streamTime,
			RtAudioStreamStatus status,
			void* context);
	};
}