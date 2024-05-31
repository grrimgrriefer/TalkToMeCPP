// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include "AudioCaptureDevice.h"
#include "AudioWebSocket.h"
#include <memory>
#include <string>
#include <thread>

namespace Utility::AudioInput
{
	/// <summary>
	/// Wrapper that handles the initialization and cleanup of
	/// the AudioWebSocket and the AudioCaptureDevice
	/// </summary>
	class AudioInputWrapper
	{
	public:
		explicit AudioInputWrapper(Logging::LoggerInterface& logger,
			std::string_view serverIP,
			int serverPort);

		void StartStreaming();
		void StopStreaming();

	private:
		AudioCaptureDevice m_audioCaptureDevice;
		std::shared_ptr<Utility::AudioInput::AudioWebSocket> m_audioWebSocket;

		std::jthread m_startupThread;
		bool m_isStreaming = false;
		bool m_isStartingUp = false;
	};
}
