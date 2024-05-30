// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Params specific to the voice service of a character.
	/// NOTE: Might have to extend this, I only tested with Coqui
	/// </summary>
	struct VoiceServiceParams
	{
		const std::string m_filename;
		const std::string m_temperature;
		const std::string m_topK = nullptr;
		const std::string m_topP = nullptr;

		explicit VoiceServiceParams(std::string_view filename,
				std::string_view temperature,
				std::string_view topK,
				std::string_view topP) :
			m_filename(filename),
			m_temperature(temperature),
			m_topK(topK),
			m_topP(topP)
		{
		}
	};
}