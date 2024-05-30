// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "VoiceServiceParams.h"
#include <string>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Contains data informing other systems of which voice service & parameters to use
	/// for a specific character.
	/// NOTE: This is tied to a character, this is NOT generic.
	/// </summary>
	struct CharVoiceService
	{
		const VoiceServiceParams m_parameters;
		const std::string m_name;
		const std::string m_id;

		explicit CharVoiceService(const VoiceServiceParams& params,
				std::string_view id,
				std::string_view name) :
			m_parameters(params),
			m_name(name),
			m_id(id)
		{
		}
	};
}