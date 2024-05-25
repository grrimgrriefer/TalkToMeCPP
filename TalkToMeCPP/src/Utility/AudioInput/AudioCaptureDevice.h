// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "MicrophoneWebSocket.h"
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
		AudioCaptureDevice(std::shared_ptr<MicrophoneWebSocket> socket) :
			microphoneApi(std::make_unique<RtAudio>()),
			m_socket(socket)
		{
		}

		~AudioCaptureDevice()
		{
			if (microphoneApi)
			{
				microphoneApi->stopStream();
				if (microphoneApi->isStreamOpen()) microphoneApi->closeStream();
			}
		}

		void Initialize()
		{
			if (microphoneApi->getDeviceCount() == 0)
			{
				std::cerr << "No audio devices available\n";
				return;
			}

			RtAudio::StreamParameters parameters;
			parameters.deviceId = microphoneApi->getDefaultInputDevice();
			parameters.nChannels = 1;
			parameters.firstChannel = 0;

			unsigned int sampleRate = 16000;
			unsigned int bufferFrames = 256; // 256 sample frames

			try
			{
				microphoneApi->openStream(nullptr, &parameters, RTAUDIO_SINT16,
								sampleRate, &bufferFrames, &AudioCaptureDevice::audioCallback, this);
			}
			catch (std::exception& e)
			{
				std::cout << e.what();
				return;
			}
		}

		void startStream(void (MicrophoneWebSocket::* callback)(const char* buffer, unsigned int nBufferFrames))
		{
			if (microphoneApi && !microphoneApi->isStreamRunning())
			{
				m_callback = std::bind(&MicrophoneWebSocket::SendData, m_socket.get(), std::placeholders::_1, std::placeholders::_2);
				microphoneApi->startStream();
			}
		}

		/*void stopStream()
		{
			if (microphoneApi && microphoneApi->isStreamRunning())
			{
				m_callback = nullptr;
				microphoneApi->stopStream();
			}
		}*/

		static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
								 double streamTime, RtAudioStreamStatus status, void* context)
		{
			if (status)
			{
				std::cerr << "Stream overflow detected!\n";
			}
			const auto* buffer = static_cast<char*>(inputBuffer);
			auto* instance = static_cast<AudioCaptureDevice*>(context);
			instance->ReceiveAudioInputData(buffer, nBufferFrames);

			return 0;
		}

		void ReceiveAudioInputData(const char* buffer, unsigned int nBufferFrames)
		{
			if (m_callback)
			{
				m_callback(buffer, nBufferFrames);
			}
		}

	private:
		std::unique_ptr<RtAudio> microphoneApi;
		std::shared_ptr<MicrophoneWebSocket> m_socket;
		std::function<void(const char*, unsigned int)> m_callback;
		//void(MicrophoneWebSocket::* m_callback)(const char* buffer, unsigned int nBufferFrames);
	};
}