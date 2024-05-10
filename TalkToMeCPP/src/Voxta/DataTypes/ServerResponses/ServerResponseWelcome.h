// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ServerResponseBase.h"
#include "../CharData.h"
#include <string>

namespace Voxta::DataTypes::ServerResponses
{
	struct ServerResponseWelcome : ServerResponseBase
	{
		ServerResponseType GetType() final
		{
			return ServerResponseType::WELCOME;
		}

		explicit ServerResponseWelcome(const DataTypes::CharData& userData) : m_user(userData)
		{
		}
		DataTypes::CharData m_user;
	};
}