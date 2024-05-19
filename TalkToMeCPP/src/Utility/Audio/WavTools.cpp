// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "WavTools.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <string>
#include <format>

namespace Utility::Audio
{
	WavTools::WavTools(Logging::LoggerInterface& logger)
		: m_logger(logger)
	{
	}

	double WavTools::CalculateDuration(const std::string& localPath) const
	{
		std::ifstream file(localPath, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Error: Could not open WAV file." << std::endl;
			return -1.0; // Error code
		}

		WAVHeader header;
		file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
		if (file.gcount() != sizeof(WAVHeader))
		{
			std::cerr << "Error: Could not read WAV header." << std::endl;
			return -1.0; // Error code
		}

		// Skip over any extra header information
		file.seekg(header.subchunk1Size - 16, std::ios::cur);

		// Read the Subchunk2ID and Subchunk2Size
		char subchunk2ID[4];
		uint32_t subchunk2Size;
		file.read(subchunk2ID, sizeof(subchunk2ID));
		file.read(reinterpret_cast<char*>(&subchunk2Size), sizeof(subchunk2Size));

		double duration = static_cast<double>(subchunk2Size) / (header.sampleRate * header.numChannels * (header.bitsPerSample / 8.0));

		return duration;
	}
}