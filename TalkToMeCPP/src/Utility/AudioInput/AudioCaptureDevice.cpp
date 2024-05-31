// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioCaptureDevice.h"
#include "AudioWebSocket.h"
#include "../Logging/LoggerInterface.h"
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <rtaudio/RtAudio.h>
#include <mutex>

namespace Utility::AudioInput
{
	AudioCaptureDevice::AudioCaptureDevice(Logging::LoggerInterface& logger) :
		m_logger(logger)
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

	bool AudioCaptureDevice::IsInitialized()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		return microphoneApi->isStreamOpen();
	}

	bool AudioCaptureDevice::TryInitialize()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		if (microphoneApi->getDeviceCount() == 0)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error, "No audio devices available");
			return false;
		}

		if (microphoneApi->isStreamOpen())
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Warning,
				"Audio input stream is already open, concurrent audio streams are not allowed.");
			return false;
		}

		RtAudio::StreamParameters parameters;
		parameters.deviceId = microphoneApi->getDefaultInputDevice();
		parameters.nChannels = 1;
		parameters.firstChannel = 0;
		unsigned int sampleRate = 16000;
		unsigned int bufferFrames = 256;

		try
		{
			microphoneApi->openStream(nullptr, &parameters, RTAUDIO_SINT16,
							sampleRate, &bufferFrames, &AudioCaptureDevice::AudioCallback, this);
			auto info = microphoneApi->getDeviceInfo(parameters.deviceId);
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Info,
				std::format("Started audio input stream from device {}", info.name));
			return true;
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			return false;
		}
	}

	void AudioCaptureDevice::StartStream()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		if (microphoneApi && !microphoneApi->isStreamRunning())
		{
			microphoneApi->startStream();
		}
	}

	void AudioCaptureDevice::StopStream()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		if (microphoneApi && microphoneApi->isStreamRunning())
		{
			microphoneApi->stopStream();
		}
	}

	void AudioCaptureDevice::ReceiveAudioInputData(const char* buffer, unsigned int nBufferFrames)
	{
		m_socket->Send(buffer, nBufferFrames);
	}

	int AudioCaptureDevice::AudioCallback(void* outputBuffer,
		void* inputBuffer,
		unsigned int nBufferFrames,
		double streamTime,
		RtAudioStreamStatus status,
		void* context)
	{
		auto* instance = static_cast<AudioCaptureDevice*>(context);
		if (status)
		{
			instance->m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Warning,
				"Stream overflow detected!");
		}
		const auto* buffer = static_cast<char*>(inputBuffer);
		instance->ReceiveAudioInputData(buffer, nBufferFrames);

		return 0;
	}
}