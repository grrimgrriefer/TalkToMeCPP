// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include <cstdint>
#include <vector>

namespace Utility::AudioPlayback
{
	/// <summary>
	/// Utility class for handling binary data that represents wav data
	/// </summary>
	class WavTools
	{
	public:
		explicit WavTools(Logging::LoggerInterface& logger);
		double CalculateDuration(const std::vector<char>& audioData) const;

	private:
		struct WAVHeader
		{
			char chunkID[4];
			uint32_t chunkSize;
			char format[4];
			char subchunk1ID[4];
			uint32_t subchunk1Size;
			uint16_t audioFormat;
			uint16_t numChannels;
			uint32_t sampleRate;
			uint32_t byteRate;
			uint16_t blockAlign;
			uint16_t bitsPerSample;
		};

		Logging::LoggerInterface& m_logger;

		int ReportFailure() const;
	};
}