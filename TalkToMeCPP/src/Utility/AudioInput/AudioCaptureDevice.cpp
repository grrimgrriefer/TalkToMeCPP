// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioCaptureDevice.h"
#include "AudioWebSocket.h"
#include <iostream>
#include <rtaudio/RtAudio.h>
#include <memory>
#include <exception>
#include <functional>

namespace Utility::AudioInput
{
	AudioCaptureDevice::AudioCaptureDevice() : microphoneApi(std::make_unique<RtAudio>())
	{
	}

	AudioCaptureDevice::~AudioCaptureDevice()
	{
		if (microphoneApi)
		{
			microphoneApi->stopStream();
			if (microphoneApi->isStreamOpen()) microphoneApi->closeStream();
		}
	}

	void AudioCaptureDevice::RegisterSocket(std::shared_ptr<Utility::AudioInput::AudioWebSocket> socket)
	{
		m_socket = socket;
	}

	void AudioCaptureDevice::Initialize()
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

	void AudioCaptureDevice::startStream()
	{
		if (microphoneApi && !microphoneApi->isStreamRunning())
		{
			microphoneApi->startStream();
		}
	}

	void AudioCaptureDevice::stopStream()
	{
		if (microphoneApi && microphoneApi->isStreamRunning())
		{
			microphoneApi->stopStream();
		}
	}

	int AudioCaptureDevice::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
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

	void AudioCaptureDevice::ReceiveAudioInputData(const char* buffer, unsigned int nBufferFrames)
	{
		m_socket->send(buffer, nBufferFrames);
	}
}