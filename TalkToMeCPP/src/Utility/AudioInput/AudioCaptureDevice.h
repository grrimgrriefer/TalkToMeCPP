// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioWebSocket.h"
#include <iostream>
#include <rtaudio/RtAudio.h>
#include <memory>
#include <exception>
#include <functional>

namespace Utility::AudioInput
{
	class AudioCaptureDevice
	{
	public:
		AudioCaptureDevice();
		~AudioCaptureDevice();

		static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
						 double streamTime, RtAudioStreamStatus status, void* context);

		void RegisterSocket(std::shared_ptr<Utility::AudioInput::AudioWebSocket> socket);
		void Initialize();
		void startStream();
		void stopStream();
		void ReceiveAudioInputData(const char* buffer, unsigned int nBufferFrames);

	private:
		std::unique_ptr<RtAudio> microphoneApi;
		std::shared_ptr<Utility::AudioInput::AudioWebSocket> m_socket;
	};
}