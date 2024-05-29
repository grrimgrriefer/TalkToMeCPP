// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioCaptureDevice.h"
#include "AudioWebSocket.h"
#include <memory>
#include <string>

namespace Utility::AudioInput
{
	class AudioInputWrapper
	{
	public:
		explicit AudioInputWrapper(std::string_view serverIP, int serverPort);

		void StartStreaming();
		void StopStreaming();

	private:
		std::shared_ptr<Utility::AudioInput::AudioWebSocket> websocketPotato;
		AudioCaptureDevice audioDevice;
		std::jthread startupThread;
		bool isStreaming = false;
		bool isStartingUp = false;
	};
}
