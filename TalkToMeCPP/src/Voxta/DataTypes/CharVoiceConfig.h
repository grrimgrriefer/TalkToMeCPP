// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string_view>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Helper data struct for ServerResponseCharacterLoaded
	/// C++ still not allowing nested structs FML
	/// </summary>
	struct CharVoiceConfig
	{
		explicit CharVoiceConfig(std::string_view backend, std::string_view voiceId,
			std::string_view voiceLabel, std::string_view serviceName) : m_backend(backend), m_voiceId(voiceId),
			m_voiceLabel(voiceLabel), m_serviceName(serviceName)
		{
		}

		const std::string m_backend;
		const std::string m_voiceId;
		const std::string m_voiceLabel;
		const std::string m_serviceName;
	};
}