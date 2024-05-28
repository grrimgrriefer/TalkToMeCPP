// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioWebSocket.h"
#include "AudioCaptureDevice.h"
#include <memory>

namespace Utility::AudioInput
{
	class AudioInputWrapper
	{
	public:
		explicit AudioInputWrapper(std::string_view serverIP, int serverPort);

		void OpenAudioSocket();
		void ConnectMicrophoneSocket();
		void RegisterPotato();
		void StartStreaming();
		void StopStreaming();

	private:
		std::shared_ptr<Utility::AudioInput::AudioWebSocket> websocketPotato;
		AudioCaptureDevice audioDevice;
		bool isStreaming = false;
		bool isInitialized = false;
	};
}
