// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "WebsocketPotato.h"
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
		AudioCaptureDevice() :
			microphoneApi(std::make_unique<RtAudio>())
			//m_socket(socket)
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

		void RegisterSocket(std::shared_ptr<Utility::AudioInput::websocket_endpoint> socket)
		{
			m_socket = socket;
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

				auto info = microphoneApi->getDeviceInfo(parameters.deviceId);
				// Print, for example, the name and maximum number of output channels for each device
				std::cout << std::format("Started audio input stream from device {}", info.name) << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << e.what();
				return;
			}
		}

		void startStream()
		{
			if (microphoneApi && !microphoneApi->isStreamRunning())
			{
				microphoneApi->startStream();
			}
		}

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
			m_socket->send(0, buffer, nBufferFrames);
		}

	private:
		std::unique_ptr<RtAudio> microphoneApi;
		std::shared_ptr<Utility::AudioInput::websocket_endpoint> m_socket;
	};
}