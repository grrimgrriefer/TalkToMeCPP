// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include "WavTools.h"
#include <cstdint>
#include <vector>
#include <stdexcept>

namespace Utility::AudioPlayback
{
	WavTools::WavTools(Logging::LoggerInterface& logger) :
		m_logger(logger)
	{
	}

	double WavTools::CalculateDuration(const std::vector<char>& audioData) const
	{
		if (audioData.size() < sizeof(WAVHeader))
		{
			return ReportFailure();
		}

		WAVHeader header;
		std::memcpy(&header, audioData.data(), sizeof(WAVHeader));

		size_t dataOffset = sizeof(WAVHeader) + (header.subchunk1Size - 16);

		// Check for the 'data' subchunk
		char subchunk2ID[4];
		if (audioData.size() < dataOffset + sizeof(subchunk2ID))
		{
			return ReportFailure();
		}

		uint32_t subchunk2Size;
		std::memcpy(&subchunk2Size, audioData.data() + dataOffset + sizeof(subchunk2ID), sizeof(subchunk2Size));

		return static_cast<double>(subchunk2Size) / (header.sampleRate *
			header.numChannels * (header.bitsPerSample / 8.0));
	}

	int WavTools::ReportFailure() const
	{
		m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error, "Error: Incomplete WAV data.");
		throw std::range_error("Error: Incomplete WAV data.");
		return -1;
	}
}