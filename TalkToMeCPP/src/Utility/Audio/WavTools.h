// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>

namespace Utility::Audio
{
	class WavTools
	{
	public:
		double CalculateDuration(const std::string& localPath) const;

	private:
		struct WavHeader
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
			char subchunk2ID[4];
			uint32_t subchunk2Size;
		};
	};
}