// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "WavTools.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <string>

namespace Utility::Audio
{
	double WavTools::CalculateDuration(const std::string& localPath) const
	{
		std::ifstream file(localPath, std::ios::binary);
		WavHeader header;

		if (file.is_open())
		{
			file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
			int numSamples = header.subchunk2Size / (header.numChannels * (header.bitsPerSample / 8));
			double duration = static_cast<double>(numSamples) / header.sampleRate;
			//std::cout << "The duration of the file is: " << duration << " seconds." << std::endl;
			return duration;
		}
		else
		{
			//std::cerr << "Could not open the file." << std::endl;
			return 1;
		}
	}
}