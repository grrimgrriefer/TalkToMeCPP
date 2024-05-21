// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include <iostream>
#include <vector>
#include <rtaudio/RtAudio.h>
#include <memory>

namespace Utility::AudioInput
{
	class AudioCaptureDevice
	{
	public:
		AudioCaptureDevice() : microphoneApi(std::make_unique<RtAudio>())
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
				e.what();
				return;
			}
		}

		~AudioCaptureDevice()
		{
			if (microphoneApi)
			{
				microphoneApi->stopStream();
				if (microphoneApi->isStreamOpen()) microphoneApi->closeStream();
			}
		}

		void startStream()
		{
			if (microphoneApi && !microphoneApi->isStreamRunning())
			{
				microphoneApi->startStream();
			}
		}

		void stopStream()
		{
			if (microphoneApi && microphoneApi->isStreamRunning())
			{
				microphoneApi->stopStream();
			}
		}

		static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
								 double streamTime, RtAudioStreamStatus status, void* context)
		{
			if (status)
			{
				std::cerr << "Stream overflow detected!\n";
			}
			// Cast inputBuffer to the type expected by the audio format
			auto* buffer = static_cast<short*>(inputBuffer);

			// Use the WebSocket connection to send the audio data
			auto* instance = static_cast<AudioCaptureDevice*>(context);
			instance->ReceiveAudioInputData(buffer, nBufferFrames);

			return 0;
		}

		void ReceiveAudioInputData(short*, unsigned int nBufferFrames)
		{
		}

	private:
		std::unique_ptr<RtAudio> microphoneApi;
	};
}