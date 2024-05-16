// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include "../CharData.h"

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for Welcome response from the server.
	/// Should be pretty obvious
	/// </summary>
	struct ServerResponseWelcome : ServerResponseBase
	{
		ServerResponseType GetType() final
		{
			return ServerResponseType::WELCOME;
		}

		explicit ServerResponseWelcome(const DataTypes::CharData& userData) : m_user(userData)
		{
		}

		const DataTypes::CharData m_user;
	};
}