// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "WavTools.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <string>
#include <format>

namespace Utility::AudioPlayback
{
	WavTools::WavTools(Logging::LoggerInterface& logger)
		: m_logger(logger)
	{
	}

	double WavTools::CalculateDuration(const std::vector<char>& audioData) const
	{
		WAVHeader header;
		std::memcpy(&header, audioData.data(), sizeof(WAVHeader));

		size_t dataOffset = sizeof(WAVHeader) + (header.subchunk1Size - 16);

		// Check for the 'data' subchunk
		char subchunk2ID[4];
		if (audioData.size() < dataOffset + sizeof(subchunk2ID))
		{
			std::cerr << "Error: Incomplete WAV data." << std::endl;
			return -1.0; // Error code
		}

		uint32_t subchunk2Size;
		std::memcpy(&subchunk2Size, audioData.data() + dataOffset + sizeof(subchunk2ID), sizeof(subchunk2Size));

		double duration = static_cast<double>(subchunk2Size) / (header.sampleRate * header.numChannels * (header.bitsPerSample / 8.0));

		return duration;
	}
}