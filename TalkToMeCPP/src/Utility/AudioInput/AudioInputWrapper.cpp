// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioInputWrapper.h"
#include "AudioWebSocket.h"
#include "../Logging/LoggerInterface.h"
#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace Utility::AudioInput
{
	AudioInputWrapper::AudioInputWrapper(Logging::LoggerInterface& logger,
			std::string_view serverIP,
			int serverPort) :
		m_audioCaptureDevice(logger),
		m_audioWebSocket(std::make_shared<Utility::AudioInput::AudioWebSocket>(logger, serverIP, serverPort))
	{
	}

	void AudioInputWrapper::StartStreaming()
	{
		if (m_isStartingUp || m_isStreaming)
		{
			return;
		}

		if (!m_audioCaptureDevice.IsInitialized())
		{
			m_isStartingUp = true;
			m_startupThread = std::jthread([this] ()
				{
					m_audioWebSocket->Connect();
					std::this_thread::sleep_for(std::chrono::milliseconds(500));

					m_audioWebSocket->Send("{\"contentType\":\"audio/wav\",\"sampleRate\":16000,"
						"\"channels\":1,\"bitsPerSample\": 16,\"bufferMilliseconds\":30}");
					std::this_thread::sleep_for(std::chrono::milliseconds(500));

					m_audioCaptureDevice.RegisterSocket(m_audioWebSocket);
					m_audioCaptureDevice.TryInitialize();
					std::this_thread::sleep_for(std::chrono::milliseconds(500));

					m_audioCaptureDevice.StartStream();
					m_isStreaming = true;
					m_isStartingUp = false;
				});
		}
		else
		{
			m_audioCaptureDevice.StartStream();
			m_isStreaming = true;
		}
	}

	void AudioInputWrapper::StopStreaming()
	{
		if (m_isStartingUp || !m_isStreaming)
		{
			return;
		}

		m_audioCaptureDevice.StopStream();
		m_isStreaming = false;
	}
}