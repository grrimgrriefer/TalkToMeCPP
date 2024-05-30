// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServiceData.h"
#include <map>
#include <string>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Data struct extending voice service with extra required parameters for a
	/// specific character voice.
	/// </summary>
	struct VoiceServiceData : public ServiceData
	{
		const std::map<std::string, std::string> m_voiceParameters;
		const std::string m_voiceLabel;

		explicit VoiceServiceData(const std::map<std::string, std::string>& parameters,
				std::string_view voiceLabel,
				std::string_view serviceName,
				std::string_view serviceId) :
			ServiceData(ServiceData::ServiceType::TEXT_TO_SPEECH, serviceName, serviceId),
			m_voiceParameters(parameters),
			m_voiceLabel(voiceLabel)
		{
		}
	};
}