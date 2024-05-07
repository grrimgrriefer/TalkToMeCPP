// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaResponseBase.h"
#include "CharData.h"
#include <string>

namespace Voxta::DataTypes
{
	struct VoxtaResponseWelcome : DataTypes::VoxtaResponseBase
	{
		DataTypes::VoxtaResponseType GetType() final
		{
			return DataTypes::VoxtaResponseType::WELCOME;
		}

		explicit VoxtaResponseWelcome(const DataTypes::CharData& userData) : m_user(userData)
		{}

		DataTypes::CharData m_user;
	};
}